/*
 * S32K_UART.h
 *
 *  Created on: 22 mar. 2025
 *      Author: Juan.Bueno
 */


#ifndef __S32K_UART_H__
#define __S32K_UART_H__

#include "S32K_PLL.h"

//==================================
//               UART
//==================================
//      channel     port       options
#define UART0_RX    PTB0       //PTA2 PTB0 PTC2
#define UART0_TX    PTB1       //PTA3 PTB1 PTC3

#define UART1_RX    PTC8       //PTC6 PTC8 PTD13
#define UART1_TX    PTC9       //PTC7 PTC9 PTD14

#define UART2_RX    PTA8       //PTD6 PTD17 PTA8
#define UART2_TX    PTA9       //PTD7 PTE12 PTA9

/*
 * @brief: Initialize IO Port
 * @param: LPUART0, LPUART1, LPUART2
 */
void UART_PinInit(LPUART_Type *base);

/*
 * @brief: Initialize serial port
 * @param: base, LPUART0, LPUART1, LPUART2
 * @param: baud: frequency
 * @note: 81N
 */
void UART_Init (LPUART_Type *base, uint32 baud);

/*
 * @brief: Send char via UART
 * @param: base, LPUART0, LPUART1, LPUART2
 * @param: chr, char to send
 */
void UART_PutChar(LPUART_Type *base, uint8_t chr);

/*
 * @brief: Send buffer via UART
 * @param: base, LPUART0, LPUART1, LPUART2
 * @param: buff, pointer to buffer
 * @param: len, length
 */
void UART_PutBuff(LPUART_Type *base, uint8_t *buff, uint32_t len);

/*
 * @brief: Send string via UART
 * @param: base, LPUART0, LPUART1, LPUART2
 * @param: str, pointer to string
 */
void UART_PutStr(LPUART_Type *base, uint8_t *str);

/*
 * @brief: receive a char
 * @param: base, LPUART0, LPUART1, LPUART2
 */
uint8_t UART_GetChar(LPUART_Type *base);


#endif // __S32K_UART_H__
