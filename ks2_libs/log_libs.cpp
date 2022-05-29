#define register // Deprecated in C++11. remove warning

#ifndef LOG_LIBS_NO_CONFIG_H
    #include "config.h"
#endif
#include "log_libs.h"

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "macro_libs.h"
#include "compiler_libs.h"

#if !defined(LOG_LIBS_NO_RTT)
    #include "SEGGER_RTT.h"
    struct SeggerRttInit_t
    {// constructor only to init before main
        SeggerRttInit_t(){SEGGER_RTT_Init();}
    };
static SeggerRttInit_t segger;
#endif
static_assert(LOG_LIBS_PRINTF_BUFFER_SIZE >= 32);

#ifndef LOG_LIBS_BUFFER_PLACEMENT
    #define LOG_LIBS_BUFFER_PLACEMENT
#endif
#ifndef LOG_LIBS_TX_BUFFER_SIZE
    #define LOG_LIBS_TX_BUFFER_SIZE (0x100)
#endif
static_assert(KS2_POWER_OF_TWO(LOG_LIBS_TX_BUFFER_SIZE), "tx buff size is not power of 2!");

#ifndef LOG_LIBS_RX_BUFFER_SIZE
    #define LOG_LIBS_RX_BUFFER_SIZE (0x10)
#endif
static_assert(KS2_POWER_OF_TWO(LOG_LIBS_RX_BUFFER_SIZE), "rx buff size is not power of 2!");

class RingBuffer_t
{
public:
    RingBuffer_t(char* inBufferP, size_t inSize):
        bufferP(inBufferP), itWrite(0), itRead(0), size(inSize), mask(inSize-1){}
    char   Read();
    void   Write(const char inData);
    size_t Write(const char* inDataP, int& inSize);
    void   IncWrite(uint16_t n){itWrite = (itWrite + n) & mask;}
    inline char* GetReadP() {return bufferP + itRead;}
    inline bool  IsEmpty()  {return (itRead == itWrite);}
    uint16_t itWrite;
    uint16_t itRead;
    const size_t size;
protected:
    char* const bufferP;
    const size_t mask;
};

size_t RingBuffer_t::Write(const char* inDataP, int& inSize)
{
    size_t leftSize = size - itWrite;
    size_t sizeToTransfer = 0U;
    const char emptyMsg[] = {"{CUT}\n"};
    size_t emptySpace = ((itRead - itWrite - 1U) & mask);
    if(static_cast<int>(emptySpace - (sizeof(emptyMsg) - 1U)) <= inSize)
    {
        inSize = sizeof(emptyMsg) - 1U;
        inDataP = emptyMsg;
        if(emptySpace <= inSize)
            return 0U;
    }
    if(itWrite >= itRead)
    {
        if(leftSize > inSize)
        {
            memcpy(bufferP + itWrite, inDataP, inSize);
            sizeToTransfer = inSize;
        }
        else
        {
            memcpy(bufferP + itWrite, inDataP, leftSize);
            memcpy(bufferP, inDataP + leftSize, inSize - leftSize);
            sizeToTransfer = leftSize;
        }
    }
    else if(itWrite < itRead)
    {
        memcpy(bufferP + itWrite, inDataP, inSize);
        sizeToTransfer = inSize;
    }
    return sizeToTransfer;
}

void RingBuffer_t::Write(const char inData)
{
    bufferP[itWrite++] = inData;
    itWrite &= mask;
}

char RingBuffer_t::Read()
{
    char retval = bufferP[itRead++];
    itRead &= mask;
    return retval;
}

static LOG_LIBS_BUFFER_PLACEMENT char txBuffer[LOG_LIBS_TX_BUFFER_SIZE];
static LOG_LIBS_BUFFER_PLACEMENT char rxBuffer[LOG_LIBS_RX_BUFFER_SIZE];
static RingBuffer_t txRingBuffer(txBuffer, sizeof(txBuffer));
static RingBuffer_t rxRingBuffer(rxBuffer, sizeof(rxBuffer));
static uint16_t transferedPos = 0;    //pos for DMA IRQ

extern "C" __WEAK void LogLibsSend(char* dataP, size_t size){}
extern "C" __WEAK void LogLibsPrintCustom(char* buff, int n){}


static void PrintUart(char* buff, int n)
{
    auto sizeToTransfer = txRingBuffer.Write(buff, n);
    auto leftSize = txRingBuffer.size - txRingBuffer.itWrite;
    if(sizeToTransfer)
    {
        LOG_LIBS_DEFINE_CRITICAL_SEND(critical);
        LOG_LIBS_ENTER_CRITICAL_SEND(critical);
            bool isEmpty = txRingBuffer.IsEmpty();
            txRingBuffer.IncWrite(n);
            if(isEmpty)
            { // IRQ/DMA finished transfer. Need to re-init it.
                if(leftSize < n)
                {// overflow
                    transferedPos = 0;
                    #ifdef LOG_LIBS_MAX_UART_TRANSMIT
                    if(leftSize > LOG_LIBS_MAX_UART_TRANSMIT)
                        transferedPos = txRingBuffer.itRead + LOG_LIBS_MAX_UART_TRANSMIT;
                    #endif
                }
                else
                {// not overflow
                    transferedPos = txRingBuffer.itWrite;
                    #ifdef LOG_LIBS_MAX_UART_TRANSMIT
                    if(n > LOG_LIBS_MAX_UART_TRANSMIT)
                        transferedPos = txRingBuffer.itRead + LOG_LIBS_MAX_UART_TRANSMIT;
                    #endif
                }
                LogLibsSend(txRingBuffer.GetReadP(), sizeToTransfer);
            }
        LOG_LIBS_EXIT_CRITICAL_SEND(critical);
    }
}

static void LogLibsPrint(char* buff, int n)
{
    LOG_LIBS_DEFINE_CRITICAL(critical);
    #if !defined(LOG_LIBS_NO_RTT)
        LOG_LIBS_ENTER_CRITICAL(critical);
            SEGGER_RTT_WriteNoLock(0, buff, n);
        LOG_LIBS_EXIT_CRITICAL(critical);
    #endif
    #if defined(LOG_LIBS_PRINT_UART)
        LOG_LIBS_ENTER_CRITICAL(critical);
            PrintUart(buff, n);
        LOG_LIBS_EXIT_CRITICAL(critical);
    #endif
    #if defined(LOG_LIBS_PRINT_CUSTOM)
        LOG_LIBS_ENTER_CRITICAL(critical);
            LogLibsPrintCustom(buff, n);
        LOG_LIBS_EXIT_CRITICAL(critical);
    #endif
}

extern "C" __WEAK size_t LogLibsGetCurTime()
{
#if defined(FREERTOS)
    return xTaskGetTickCount();
#else
    return 0;
#endif
}

extern "C" __WEAK size_t LogLibsPrintTime(char* str)
{
    size_t curTime = LogLibsGetCurTime();
    size_t hours       = curTime / static_cast<size_t>(3600000U);

    curTime %= static_cast<size_t>(3600000U);
    size_t minutes     = curTime / static_cast<size_t>(60000U);

    curTime %= static_cast<size_t>(60000U);
    size_t seconds     = curTime / static_cast<size_t>(1000U);
    size_t miliseconds = curTime % static_cast<size_t>(1000U);

    return sprintf(str, "%03u.%02u.%02u.%03u: ", hours, minutes, seconds, miliseconds);
}

static int VLogLibsPrintCommon(char* buff, size_t buffSize, size_t offset, const char* format, va_list args)
{
    static const char ERR_STR[] = "!ERR!" LOG_LIBS_NEW_LINE;
    static const char CUT_STR[] = "!CUT!" LOG_LIBS_NEW_LINE;
    int n = vsnprintf(buff + offset, buffSize - offset, format, args);
    if(n < 0)
    {
        strcpy(buff, ERR_STR);
        n = sizeof(ERR_STR);
    }
    else if((n + offset) >= buffSize)
    {
        strcpy(buff + buffSize - sizeof(CUT_STR), CUT_STR);
        n = buffSize;
    }
    else
    {
        n += offset;
    }
    LogLibsPrint(buff, n);
    return n;
}

static int VLogLibsPrintFStamp(size_t buffSize, const char* format, va_list args)
{
    char buff[buffSize];
    size_t n = 0;
#ifdef LOG_LIBS_TIMESTAMP
    n = LogLibsPrintTime(buff);
#endif
    n += VLogLibsPrintCommon(buff, sizeof(buff), n, format, args);
    return n;
}

static int VLogLibsPrintF(size_t buffSize, const char* format, va_list args)
{
    char buff[buffSize];
    return VLogLibsPrintCommon(buff, sizeof(buff), 0U, format, args);
}

int LogLibsPrintFStamp(size_t buffSize, const char *format,...)
{
    va_list args;
    va_start(args, format);
    int n = VLogLibsPrintFStamp(buffSize, format, args);
    va_end(args);
    return n;
}

int LogLibsPrintF(size_t buffSize, const char *format,...)
{
    va_list args;
    va_start(args, format);
    int n = VLogLibsPrintF(buffSize, format, args);
    va_end(args);
    return n;
}

#if (!defined __ICCARM__) && (!defined(LOG_LIBS_USE_PRINTF) || (defined(LOG_LIBS_USE_PRINTF) && LOG_LIBS_USE_PRINTF == 1))
extern "C" int printf(const char *fmt,...)
{
    char buff[LOG_LIBS_PRINTF_BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    int n = VLogLibsPrintCommon(buff, sizeof(buff), 0U, fmt, args);
    va_end(args);
    return n;
}
#endif

static void PrintHexByte(char*& buff, uint8_t byte)
{
    uint8_t b = byte >> 4U;
    *buff = ('0' + b) * (b < 10) + (('A'-10) + b) * (b >= 10);
    buff++;

    b = byte & 0xFU;
    *buff = ('0' + b) * (b < 10) + (('A'-10) + b) * (b >= 10);
    buff++;
}

void LogLibsPrintHex(const char* start, const char* end, uint8_t* dataP, uint32_t size, const char* fmt)
{
    static_assert(LOG_LIBS_PRINTF_BUFFER_SIZE >= 50, "Increase LOG_LIBS_PRINTF_BUFFER_SIZE for hexdump!");

    char buff[LOG_LIBS_PRINTF_BUFFER_SIZE];
    // start HEXDUMP\n
    size_t len = strlen(start);
    memcpy(buff, start, len);
    LogLibsPrint(buff, len);
    auto currBuff = buff;
    for(uint32_t i = 0; i < size; i++)
    {
        PrintHexByte(currBuff, dataP[i]);
        if(!((i+1)&0xF))
        {// next line
            *currBuff = '\n';
            currBuff++;
            LogLibsPrint(buff, currBuff - buff);
            currBuff = buff;
        }
        else
        {// next byte
            *currBuff = ' ';
            currBuff++;
        }
    }
    if(currBuff != buff)
    {
        *currBuff = '\n';
        currBuff++;
    }
    len = strlen(end);
    memcpy(currBuff, end, len);
    currBuff += len;
    if(currBuff - buff)
    {
        LogLibsPrint(buff, currBuff - buff);
    }
}

void LogLibsTranferComplete(void)
{
    txRingBuffer.itRead = transferedPos;
    if(txRingBuffer.itRead != txRingBuffer.itWrite)
    {
        size_t sizeToTransfer;
        if(txRingBuffer.itRead < txRingBuffer.itWrite)
        {// not overflow
            transferedPos = txRingBuffer.itWrite;
            sizeToTransfer = txRingBuffer.itWrite - txRingBuffer.itRead;
        }
        else
        {// overflow
            transferedPos = 0;
            sizeToTransfer = txRingBuffer.size - txRingBuffer.itRead;
        }
        #ifdef LOG_LIBS_MAX_UART_TRANSMIT
        if(sizeToTransfer > LOG_LIBS_MAX_UART_TRANSMIT)
        {
            sizeToTransfer = LOG_LIBS_MAX_UART_TRANSMIT;
            transferedPos = txRingBuffer.itRead + LOG_LIBS_MAX_UART_TRANSMIT;
        }
        #endif
        LogLibsSend(txRingBuffer.GetReadP(), sizeToTransfer);
    }
}

void LogLibsCharReceiveFromISR(uint8_t inData)
{
    rxRingBuffer.Write((char)inData);
}

int32_t LogLibsGetChar(void)
{
    int32_t key = -1;
#if !defined(LOG_LIBS_NO_RTT)
    key = SEGGER_RTT_GetKey();
#endif
    if(key < 0)
    {
#ifdef LOG_LIBS_PRINT_UART
        if(!rxRingBuffer.IsEmpty())
            key = rxRingBuffer.Read();
#endif
    }
    return key;
}