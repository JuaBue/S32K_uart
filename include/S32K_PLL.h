/*
 * S32K_PLL.h
 *
 *  Created on: 13 mar. 2025
 *      Author: Juan.Bueno
 */

#ifndef S32K_PLL_H_
#define S32K_PLL_H_

extern uint32 core_clk_M;	//MHZ
extern uint32 bus_clk_M;	//MHZ

// PLL Parameters (Phase-locked loop)
typedef enum clk_option
{
    PLLUNULL ,
    PLL100   = 100,
    PLL112   = 112,
    PLL160   = 160,			//Recomended frecuency 160
    PLL180   = 180,
    PLL184   = 184,
    PLL188   = 188,

    PLL200   = 200,
    PLL220   = 220,         //No estable
    PLL252   = 252,         //Muy inestable
    PLLtest,
} clk_option;


//==================================
//               PLL
//==================================

/*
 * @brief: CPU Initialize
 */
void MCU_Init(void);


/*
 * @brief: PLL Initialize
 * @param: opt: main frequency, (check clk_option)
 */
void SPLL_Init(clk_option opt);


/*
 * @brief: SCG Initialize
 * @note: Initialize XOSC (8MHz), SPLL (160MHz) and RUN mode
 */
void SCG_Init(void);

#endif /* S32K_PLL_H_ */
