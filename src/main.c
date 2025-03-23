/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "include.h"
#include "common.h"
#include "S32K_PLL.h"
#include "S32K_UART.h"

#if defined (__ghs__)
    #define __INTERRUPT_SVC  __interrupt
    #define __NO_RETURN _Pragma("ghs nowarning 111")
#elif defined (__ICCARM__)
    #define __INTERRUPT_SVC  __svc
    #define __NO_RETURN _Pragma("diag_suppress=Pe111")
#elif defined (__GNUC__)
    #define __INTERRUPT_SVC  __attribute__ ((interrupt ("SVC")))
    #define __NO_RETURN
#else
    #define __INTERRUPT_SVC
    #define __NO_RETURN
#endif

int main(void) {
    uint16_t count = 0U;

    SPLL_Init(PLL160);
    UART_Init(LPUART0, 115200);

    printf("This is my first test with S32K144\r\n");

    while(1) {
        printf("test printf times=%d\r\n",count);
        for(int i=0; i<0xFFFF; i++)
        {
          for(int j=0; j<0xFF; j++)
          {
            asm("nop");
          }
        }

        count++;
    }

    return 0;
}
