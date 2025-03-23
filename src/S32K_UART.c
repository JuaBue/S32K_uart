/*
 * S32K_UART.c
 *
 *  Created on: 23 mar. 2025
 *      Author: Juan.Bueno
 */

#include "include.h"
#include "S32K_UART.h"

//Set the macro _DLIB_FILE_DESCRIPTOR (Options -> Compiler C/C++ -> Preprocessor)

#if 1
// #pragma import(__use_no_semihosting)
// Support functions required by the standard library
struct __FILE
{
    int handle;
};

FILE __stdout;
//define _sys_exit() to avoid mode semihost
void _sys_exit(int x)
{
    x = x;
}
//Redefine _write function for "newlib" library to use serial port 0 as printf print port
int _write(int iFileHandle, char *pcBuffer, int iLength) {
    UART_PutBuff(LPUART0, (uint8_t *) pcBuffer, (uint32_t)iLength);
    return 0;
}

#endif


void UART_PinInit(LPUART_Type *base)
{
    if (base == LPUART0) {
        /* Enable serial port clock */
        PCC->PCCn[PCC_LPUART0_INDEX] |= PCC_PCCn_PCS(6) | PCC_PCCn_CGC_MASK;
        switch (UART0_RX) {
        case PTA2:
            PORTA->PCR[2U] = PORT_PCR_MUX(6);
            break;
        case PTB0:
            PORTB->PCR[0U] = PORT_PCR_MUX(2);
            break;
        case PTC2:
            PORTC->PCR[2U] = PORT_PCR_MUX(4);
            break;
        default:
            /* Do nothing */
            break;
        };
        switch (UART0_TX) {
        case PTA3:
            PORTA->PCR[3U] = PORT_PCR_MUX(6);
            break;
        case PTB1:
            PORTB->PCR[1U] = PORT_PCR_MUX(2);
            break;
        case PTC3:
            PORTC->PCR[3U] = PORT_PCR_MUX(4);
            break;
        default:
            /* Do nothing */
            break;
        };
    }
    else if (base == LPUART1) {
        /* Enable serial port clock */
        PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(6) | PCC_PCCn_CGC_MASK;
        switch (UART1_RX) {
        case PTC6:
            PORTC->PCR[6U] = PORT_PCR_MUX(2);
            break;
        case PTC8:
            PORTC->PCR[8U] = PORT_PCR_MUX(2);
            break;
        case PTD13:
            PORTD->PCR[13U] = PORT_PCR_MUX(3);
            break;
        default:
            /* Do nothing */
            break;
        };
        switch (UART1_TX) {
        case PTC7:
            PORTC->PCR[7U] = PORT_PCR_MUX(2);
            break;
        case PTC9:
            PORTC->PCR[9U] = PORT_PCR_MUX(2);
            break;
        case PTD14:
            PORTD->PCR[14U] = PORT_PCR_MUX(3);
            break;
        default:
            /* Do nothing */
            break;
        };
    }
    else if (base == LPUART2) {
        /* Enable serial port clock */
        PCC->PCCn[PCC_LPUART2_INDEX] |= PCC_PCCn_PCS(6)
                                     |  PCC_PCCn_CGC_MASK;
        switch (UART2_RX) {
        case PTD6:
            PORTD->PCR[6U] = PORT_PCR_MUX(2);
            break;
        case PTD17:
            PORTD->PCR[17U] = PORT_PCR_MUX(3);
            break;
        case PTA8:
            PORTA->PCR[8U] = PORT_PCR_MUX(2);
            break;
        default:
            /* Do nothing */
            break;
        };
        switch (UART2_TX) {
        case PTD7:
            PORTD->PCR[7U] = PORT_PCR_MUX(2);
            break;
        case PTE12:
            PORTE->PCR[12U] = PORT_PCR_MUX(3);
            break;
        case PTA9:
            PORTA->PCR[9U] = PORT_PCR_MUX(2);
            break;
        default:
            /* Do nothing */
            break;
        };
    }
    else {
        /* Do nothing */
    }
}


void UART_Init(LPUART_Type *base, uint32 baud)
{
    /* Use SPLL crystal clock sbr = baud_clock / ((OSR+1) × OSR_id) */
    uint16_t sbr = core_clk_M * 1000000UL / baud / 15;

    /* Enable GPIO clock. For convenience, turn on all */
    PCC->PCCn[PCC_PORTA_INDEX] = PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTB_INDEX] = PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK;

    /* Pin initialization */
    UART_PinInit(base);

    /* SBR is 13 bits, the maximum is 0x1FFF; */
    if(sbr > 0x1FFF) {
        sbr = 0x1FFF;
    }

    /* Set baud rate and open serial port */
    base->BAUD = 0x0E000000|sbr;
    base->CTRL = LPUART_CTRL_RE_MASK    // Enable reception
                |LPUART_CTRL_TE_MASK    // Enable Transmition
                |LPUART_CTRL_RIE_MASK;  // Enable receive interrupt
             // |LPUART_CTRL_PE_MASK    // Parity Enable
             // |LPUART_CTRL_PT_MASK    // Parity type: 0 for even parity, 1 for odd parity
             // |LPUART_CTRL_M_MASK;    //9-bit or 8-bit mode selection: 0 for 8-bit, 1 for 9-bit (commented to indicate 0, i.e. 8-bit) (If 9-bit, bit 8 is in UARTx_C3)
}

void UART_PutChar(LPUART_Type *base, uint8_t chr)
{
    /* Waiting for the send buffer to be empty */
    while(!(base->STAT & LPUART_STAT_TDRE_MASK));
    base->DATA = chr;
}

void UART_PutBuff(LPUART_Type *base, uint8_t *buff, uint32_t len)
{
    while(len--)
    {
        UART_PutChar(base, *buff++);
    }
}

void UART_PutStr(LPUART_Type *base, uint8_t *str)
{
    while(*str)
    {
        UART_PutChar(base, *str++);
    }
}

uint8_t UART_GetChar(LPUART_Type *base)
{
    while(!(base->STAT & LPUART_STAT_RDRF_MASK));
    return base->DATA;
}

