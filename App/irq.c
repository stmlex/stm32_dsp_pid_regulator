//#include "irq.h"
#include "config.h"
#include "log_libs.h"

//#include "stm32f4xx_ll_dma.h"
//#include "stm32f4xx_ll_usart.h"

/*
void DMA1_Stream6_IRQHandler(void)
{
    if(LL_DMA_IsActiveFlag_TC6(LOG_DMA))
    {
        LL_DMA_DisableStream(LOG_DMA, LOG_DMA_STREAM);
        LL_DMA_ClearFlag_TC6(LOG_DMA);
        LogLibsTranferComplete();
    }
}

void LogLibsSend(char* dataP, size_t size)
{
    LL_DMA_SetMemoryAddress(LOG_DMA, LOG_DMA_STREAM, (uint32_t)dataP);
    LL_DMA_SetDataLength(LOG_DMA, LOG_DMA_STREAM, size);
    LL_DMA_EnableStream(LOG_DMA, LOG_DMA_STREAM);
}

void USART2_IRQHandler(void)
{
    if(LL_USART_IsActiveFlag_RXNE(LOG_UART))
    {
        LL_USART_ClearFlag_RXNE(LOG_UART);
        LogLibsCharReceiveFromISR(LL_USART_ReceiveData8(LOG_UART));
    }
}
*/
char* ErrTask;

void vApplicationStackOverflowHook( TaskHandle_t xTask, char* pcTaskName )
{
    ErrTask = pcTaskName;
    while(1){}
}

void vApplicationTickHook(void) {
}
