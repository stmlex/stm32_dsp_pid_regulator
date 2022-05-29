#ifndef __LOG_LIBS_H
#define __LOG_LIBS_H

#include <stdint.h>
#include <stddef.h>

/*  @brief This file can be used to write logs to standart printf() or NRF_LOG()
 *
 *  @note in some cases for safe print from multiple threads or ISR user should define:
 *      LOG_LIBS_ENTER_CRITICAL() & LOG_LIBS_EXIT_CRITICAL()
 *  @example
 *  in config.h:
 *
 *      #define NRF_LOG_TO_PRINTF     ///< need to be defined if you want to printf() logs in nordic
 *
 *      #define LOG_LEVEL_MY_MODULE 2 ///< 5 == develop(printf mode only)
 *                                    ///< 4 == debug
 *                                    ///< 3 == info
 *                                    ///< 2 == warning
 *                                    ///< 1 == error
 *                                    ///< 0 == no logs
 *
 *  in my_module.c:
 *
 *      #define LOG_MODULE_NAME my_module ///< visible name in logs
 *      #if defined(LOG_LEVEL_MY_MODULE)
 *      #define LOG_MODULE_LEVEL LOG_LEVEL_MY_MODULE
 *      #endif
 *      #include "log_libs.h"
 *      #if defined(NRF52)&&!defined(NRF_LOG_TO_PRINTF)
 *      #include "nrf_log.h"
 *      NRF_LOG_MODULE_REGISTER();
 *      #endif
 *
 *      printf custom:
 *      1. Put LogLibsTranferComplete() in tranfer complete handler;
 *      2. Write LogLibsSend(). It must send n bytes to custom from some address;
 *      2. config.h:#define LOG_LIBS_PRINT_CUSTOM
 *                  #define LOG_LIBS_BUFFER_SIZE
 *
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(FREERTOS)
    #include "rtos_libs.h"
#endif

#ifndef LOG_LIBS_PRINTF_BUFFER_SIZE
    #define LOG_LIBS_PRINTF_BUFFER_SIZE (80U)
#endif

int LogLibsPrintF(size_t buffSize, const char *format,...);
int LogLibsPrintFStamp(size_t buffSize, const char *format,...);
int LogLibsSizedPrintF(size_t size, const char *format,...);
void LogLibsPrintHex(const char* start, const char* end, uint8_t* dataP, uint32_t size, const char* fmt);

void LogLibsTranferComplete(void);
void LogLibsCharReceiveFromISR(uint8_t inData);
int32_t LogLibsGetChar(void);

#ifdef LOG_LIBS_PRINT_LEN
    #define LOG_LIBS_PRINTF(...)       LogLibsPrintF     (LOG_LIBS_PRINT_LEN, ##__VA_ARGS__);
    #define LOG_LIBS_PRINTF_STAMP(...) LogLibsPrintFStamp(LOG_LIBS_PRINT_LEN, ##__VA_ARGS__);
#else
    #define LOG_LIBS_PRINTF(...)       LogLibsPrintF     (LOG_LIBS_PRINTF_BUFFER_SIZE, ##__VA_ARGS__);
    #define LOG_LIBS_PRINTF_STAMP(...) LogLibsPrintFStamp(LOG_LIBS_PRINTF_BUFFER_SIZE, ##__VA_ARGS__);
#endif

#define LOG_LIBS_STRINGIFY_(s) #s
#define LOG_LIBS_STRINGIFY(s)  LOG_LIBS_STRINGIFY_(s)

#ifndef LOG_MODULE_NAME
    #define LOG_MODULE_NAME app
#endif
#if !defined(LOG_MODULE_LEVEL)
    #ifdef NDEBUG
        #define LOG_MODULE_LEVEL 2
    #else
        #define LOG_MODULE_LEVEL 5
    #endif
#endif

#if !defined(LOG_LIBS_NEW_LINE)
    #define LOG_LIBS_NEW_LINE "\n"
#endif

#if (__cplusplus>=201703L)||(defined(__ICCARM__)&&(__cplusplus>=201402L))
    #define LOG_LIBS_IF_CONST if constexpr
#else  // not C++ or old C++
    #define LOG_LIBS_IF_CONST if
#endif

#if defined(NRF52)&&!defined(NRF_LOG_TO_PRINTF)
    #define NRF_LOG_MODULE_NAME LOG_MODULE_NAME
    #if(LOG_MODULE_LEVEL > 4)
        #define NRF_LOG_LEVEL 4
    #else
        #define NRF_LOG_LEVEL LOG_MODULE_LEVEL
    #endif
    #define LOG_ERROR(...)   do{NRF_LOG_ERROR  (__VA_ARGS__);}while(0)
    #define LOG_WARNING(...) do{NRF_LOG_WARNING(__VA_ARGS__);}while(0)
    #define LOG_INFO(...)    do{NRF_LOG_INFO   (__VA_ARGS__);}while(0)
    #define LOG_DEBUG(...)   do{NRF_LOG_DEBUG  (__VA_ARGS__);}while(0)
    #define LOG_DEVELOP(...) do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>4) {NRF_LOG_DEBUG  (__VA_ARGS__); }}while(0)

    #define LOG_RAW_ERROR(...)   do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>0) {NRF_LOG_RAW_INFO(__VA_ARGS__); }}while(0)
    #define LOG_RAW_WARNING(...) do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>1) {NRF_LOG_RAW_INFO(__VA_ARGS__); }}while(0)
    #define LOG_RAW_INFO(...)    do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>2) {NRF_LOG_RAW_INFO(__VA_ARGS__); }}while(0)
    #define LOG_RAW_DEBUG(...)   do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>3) {NRF_LOG_RAW_INFO(__VA_ARGS__); }}while(0)
    #define LOG_RAW_DEVELOP(...) do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>4) {NRF_LOG_RAW_INFO(__VA_ARGS__); }}while(0)

    #define LOG_HEXDUMP_ERROR(DATA_P, SIZE)   do{NRF_LOG_HEXDUMP_ERROR  (DATA_P, SIZE);}while(0)
    #define LOG_HEXDUMP_WARNING(DATA_P, SIZE) do{NRF_LOG_HEXDUMP_WARNING(DATA_P, SIZE);}while(0)
    #define LOG_HEXDUMP_INFO(DATA_P, SIZE)    do{NRF_LOG_HEXDUMP_INFO   (DATA_P, SIZE);}while(0)
    #define LOG_HEXDUMP_DEBUG(DATA_P, SIZE)   do{NRF_LOG_HEXDUMP_DEBUG  (DATA_P, SIZE);}while(0)
    #define LOG_HEXDUMP_DEVELOP(DATA_P, SIZE) do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>4) {NRF_LOG_HEXDUMP_DEBUG  (DATA_P, SIZE); }}while(0)
#else

    #ifdef NRF_LOG_PUSH
        #undef NRF_LOG_PUSH
    #endif
    #ifdef NRF_LOG_FLUSH
        #undef NRF_LOG_FLUSH
    #endif
    #define NRF_LOG_PUSH(a) a
    #define NRF_LOG_FLUSH() do{}while(0)

    #if (defined(LOG_LIBS_PRINT_CUSTOM)||defined(LOG_LIBS_PRINT_UART))&&!defined(LOG_LIBS_NO_COLOR)
        #define LOG_LIBS_NO_COLOR
    #endif

    #if !defined(RTT_CTRL_RESET)||defined(LOG_LIBS_NO_COLOR)
        #define LOG_LIBS_CTRL_RESET ""
    #else
        #define LOG_LIBS_CTRL_RESET RTT_CTRL_RESET
    #endif

    #if !defined(RTT_CTRL_TEXT_RED)||defined(LOG_LIBS_NO_COLOR)
        #define LOG_LIBS_CTRL_RED ""
    #else
        #define LOG_LIBS_CTRL_RED RTT_CTRL_TEXT_BRIGHT_RED
    #endif

    #if !defined(RTT_CTRL_TEXT_YELLOW)||defined(LOG_LIBS_NO_COLOR)
        #define LOG_LIBS_CTRL_YELLOW ""
    #else
        #define LOG_LIBS_CTRL_YELLOW RTT_CTRL_TEXT_BRIGHT_YELLOW
    #endif

    #if !defined(RTT_CTRL_TEXT_GREEN)||defined(LOG_LIBS_NO_COLOR)
        #define LOG_LIBS_CTRL_GREEN ""
    #else
        #define LOG_LIBS_CTRL_GREEN RTT_CTRL_TEXT_BRIGHT_GREEN
    #endif

    #if !defined(LOG_LIBS_INFO_COLOR)
        #define LOG_LIBS_INFO_COLOR ""
    #endif
    #if defined(LOG_LIBS_NO_COLOR)
        #undef LOG_LIBS_INFO_COLOR
        #define LOG_LIBS_INFO_COLOR ""
    #endif

    #if !defined(LOG_LIBS_DEBUG_COLOR)
        #define LOG_LIBS_DEBUG_COLOR LOG_LIBS_INFO_COLOR
    #endif
    #if defined(LOG_LIBS_NO_COLOR)
        #undef LOG_LIBS_DEBUG_COLOR
        #define LOG_LIBS_DEBUG_COLOR ""
    #endif

    #if !defined(LOG_LIBS_DEVELOP_COLOR)
        #define LOG_LIBS_DEVELOP_COLOR LOG_LIBS_DEBUG_COLOR
    #endif
    #if defined(LOG_LIBS_NO_COLOR)
        #undef LOG_LIBS_DEVELOP_COLOR
        #define LOG_LIBS_DEVELOP_COLOR ""
    #endif

    #if defined(FREERTOS) && !defined(LOG_LIBS_PRINT_FROM_IRQ)

        #define LOG_LIBS_DEFINE_CRITICAL(x)
        #define LOG_LIBS_ENTER_CRITICAL(x)  vTaskSuspendAll()
        #define LOG_LIBS_EXIT_CRITICAL(x)   do{if(!xTaskResumeAll()) taskYIELD();}while(0)

        #define LOG_LIBS_DEFINE_CRITICAL_SEND(x)
        #define LOG_LIBS_ENTER_CRITICAL_SEND(x)  taskENTER_CRITICAL()
        #define LOG_LIBS_EXIT_CRITICAL_SEND(x)   taskEXIT_CRITICAL()

    #elif defined(FREERTOS) && defined(LOG_LIBS_PRINT_FROM_IRQ)

        #define LOG_LIBS_DEFINE_CRITICAL(x) UBaseType_t x
        #define LOG_LIBS_ENTER_CRITICAL(x) \
        do{ \
            if(IsFromISR()) \
                x = taskENTER_CRITICAL_FROM_ISR(); \
            else \
                taskENTER_CRITICAL(); \
        }while(0)
        #define LOG_LIBS_EXIT_CRITICAL(x) \
        do{ \
            if(IsFromISR()) \
                taskEXIT_CRITICAL_FROM_ISR(x); \
            else \
                taskEXIT_CRITICAL(); \
        }while(0)

        #define LOG_LIBS_DEFINE_CRITICAL_SEND(x)
        #define LOG_LIBS_ENTER_CRITICAL_SEND(x)
        #define LOG_LIBS_EXIT_CRITICAL_SEND(x)

    #endif

    #define LOG_ERROR(STR, ...)   do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>0) {LOG_LIBS_PRINTF_STAMP(LOG_LIBS_CTRL_RED      "err> " LOG_LIBS_STRINGIFY(LOG_MODULE_NAME) ": " STR LOG_LIBS_CTRL_RESET "\n", ##__VA_ARGS__); }}while(0)
    #define LOG_WARNING(STR, ...) do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>1) {LOG_LIBS_PRINTF_STAMP(LOG_LIBS_CTRL_YELLOW   "wrn> " LOG_LIBS_STRINGIFY(LOG_MODULE_NAME) ": " STR LOG_LIBS_CTRL_RESET "\n", ##__VA_ARGS__); }}while(0)
    #define LOG_INFO(STR, ...)    do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>2) {LOG_LIBS_PRINTF_STAMP(LOG_LIBS_INFO_COLOR    "inf> " LOG_LIBS_STRINGIFY(LOG_MODULE_NAME) ": " STR LOG_LIBS_CTRL_RESET "\n", ##__VA_ARGS__); }}while(0)
    #define LOG_DEBUG(STR, ...)   do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>3) {LOG_LIBS_PRINTF_STAMP(LOG_LIBS_DEBUG_COLOR   "dbg> " LOG_LIBS_STRINGIFY(LOG_MODULE_NAME) ": " STR LOG_LIBS_CTRL_RESET "\n", ##__VA_ARGS__); }}while(0)
    #define LOG_DEVELOP(STR, ...) do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>4) {LOG_LIBS_PRINTF_STAMP(LOG_LIBS_DEVELOP_COLOR "dev> " LOG_LIBS_STRINGIFY(LOG_MODULE_NAME) ": " STR LOG_LIBS_CTRL_RESET "\n", ##__VA_ARGS__); }}while(0)

    #define LOG_RAW_ERROR(...)   do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>0) {LOG_LIBS_PRINTF(__VA_ARGS__); }}while(0)
    #define LOG_RAW_WARNING(...) do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>1) {LOG_LIBS_PRINTF(__VA_ARGS__); }}while(0)
    #define LOG_RAW_INFO(...)    do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>2) {LOG_LIBS_PRINTF(__VA_ARGS__); }}while(0)
    #define LOG_RAW_DEBUG(...)   do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>3) {LOG_LIBS_PRINTF(__VA_ARGS__); }}while(0)
    #define LOG_RAW_DEVELOP(...) do{LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL>4) {LOG_LIBS_PRINTF(__VA_ARGS__); }}while(0)

    #define LOG_LIBS_HEXDUMP(DATA_P, SIZE, FORMAT, LVL_STR, LVL_NUM, LVL_COLOR) \
        do{ \
            LOG_LIBS_IF_CONST(LOG_MODULE_LEVEL > LVL_NUM){ \
                LogLibsPrintHex(LVL_COLOR LVL_STR "> " LOG_LIBS_STRINGIFY(LOG_MODULE_NAME) " HEX:" LOG_LIBS_NEW_LINE, \
                    LOG_LIBS_CTRL_RESET "", DATA_P, SIZE, FORMAT); \
            } \
        }while(0)

    #define LOG_HEXDUMP_ERROR(DATA_P, SIZE)     LOG_LIBS_HEXDUMP(DATA_P, SIZE, " %02X", "err", 0, LOG_LIBS_CTRL_RED)
    #define LOG_HEXDUMP_WARNING(DATA_P, SIZE)   LOG_LIBS_HEXDUMP(DATA_P, SIZE, " %02X", "wrn", 1, LOG_LIBS_CTRL_YELLOW)
    #define LOG_HEXDUMP_INFO(DATA_P, SIZE)      LOG_LIBS_HEXDUMP(DATA_P, SIZE, " %02X", "inf", 2, LOG_LIBS_INFO_COLOR)
    #define LOG_HEXDUMP_DEBUG(DATA_P, SIZE)     LOG_LIBS_HEXDUMP(DATA_P, SIZE, " %02X", "dbg", 3, LOG_LIBS_DEBUG_COLOR)
    #define LOG_HEXDUMP_DEVELOP(DATA_P, SIZE)   LOG_LIBS_HEXDUMP(DATA_P, SIZE, " %02X", "dev", 4, LOG_LIBS_DEVELOP_COLOR)
#endif

    #ifndef LOG_LIBS_DEFINE_CRITICAL
        #define LOG_LIBS_DEFINE_CRITICAL(x)
    #endif

    #ifndef LOG_LIBS_ENTER_CRITICAL
        #define LOG_LIBS_ENTER_CRITICAL(x)
    #endif

    #ifndef LOG_LIBS_EXIT_CRITICAL
        #define LOG_LIBS_EXIT_CRITICAL(x)
    #endif

    #ifndef LOG_LIBS_DEFINE_CRITICAL_SEND
        #define LOG_LIBS_DEFINE_CRITICAL_SEND(x)
    #endif

    #ifndef LOG_LIBS_ENTER_CRITICAL_SEND
        #define LOG_LIBS_ENTER_CRITICAL_SEND(x)
    #endif

    #ifndef LOG_LIBS_EXIT_CRITICAL_SEND
        #define LOG_LIBS_EXIT_CRITICAL_SEND(x)
    #endif

#ifdef __cplusplus
}
#endif

#endif    /*__LOG_LIBS_H*/
