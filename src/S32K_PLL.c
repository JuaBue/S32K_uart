/*
 * S32K_PLL.c
 *
 *  Created on: 22 mar. 2025
 *      Author: Juan.Bueno
 */
#include "include.h"
#include "S32K_PLL.h"

/* Core clock and Bus clock */
uint32_t core_clk_M;
uint32_t bus_clk_M;


void MCU_Init(void)
{
	//OSC_OUT = 8MHz, 8MHz EXTAL
    SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1)      // System OSC DIV1=1
                 | SCG_SOSCDIV_SOSCDIV2(1);     // System OSC DIV2=1
    SCG->SOSCCFG = SCG_SOSCCFG_RANGE(2) 		// Medium frequency range OSC 1-8MHz
	             | SCG_SOSCCFG_EREFS(1);	    // Select internal OSC
    while(SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK);  // Wait for SOSCCSR unlocked state
    SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN_MASK;     // Enable OSC
    while(!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK));    // Wait for OSC enabling and valid output
    // SPLL Configuration
    // PLL_OUT = 112MHz, 8MHz EXTAL
    SCG->SPLLDIV = SCG_SPLLDIV_SPLLDIV1(1)    // PLL DIV1=1
                 | SCG_SPLLDIV_SPLLDIV2(1);   // PLL DIV2=1
    SCG->SPLLCFG = SCG_SPLLCFG_PREDIV(0)      // PLL PREDIV=0
                 | SCG_SPLLCFG_MULT(12);      // PLL MULT=28
    while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);// Wait for SPLLCSR unlocked state
    SCG->SPLLCSR=SCG_SPLLCSR_SPLLEN_MASK;     // Enable PLL
    while(!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)); // Wait for PLL enabling and valid output
    // HCCR Configuration
    // PLL_OUT = 112MHz, 8MHz EXTAL
    SCG->HCCR = SCG_HCCR_SCS(6)               // PLL source clock
              | SCG_HCCR_DIVCORE(0)           // DIVCORE=1, Core=112MHz
              | SCG_HCCR_DIVBUS(1)            // DIVBUS=2, BUS=56MHz
              | SCG_HCCR_DIVSLOW(3);          // DIVSLOW=4, Flash=28MHz
    SMC->PMPROT=SMC_PMPROT_AHSRUN_MASK        // Allows High Speed Run
              | SMC_PMPROT_AVLP_MASK;         // Allows Very Low Power Modes
    SMC->PMCTRL=SMC_PMCTRL_RUNM(3);           // Entry to High Speed Run
    // Wait for High Speed Run mode
    while(SMC->PMSTAT != SMC_PMSTAT_PMSTAT(128)){}
}


void SPLL_Init(clk_option opt)
{
    uint8_t pll_prdiv;
    uint8_t pll_mult;

    if(opt!= PLLUNULL) {
      //clock configure PLL
      switch(opt) {
      case PLL100:
        pll_prdiv   = 0U;
        pll_mult    = 9U;
        break;
      case PLL112:
        pll_prdiv   = 0U;
        pll_mult    = 12U;      //4*28=112
        break;
      case PLL160:
        pll_prdiv   = 0U;
        pll_mult    = 24U;      //PLL =  8M * (pll_mult+16)/2/(prdiv+1)=160
        break;
      case PLL180:
        pll_prdiv   = 0U;       //45*4=180
        pll_mult    = 29U;
        break;
      case PLL184:
        pll_prdiv   = 0U;       //46*4=184
        pll_mult    = 30U;
        break;
      case PLL188:
        pll_prdiv   = 0U;       //4*47=188
        pll_mult    = 31U;
        break;

      /***********************************************/
      case PLL200:
        pll_prdiv   = 0U;       //50*4=200
        pll_mult    = 34U;
        break;
      case PLL220:
        pll_prdiv   = 0U;       //55*4=220
        pll_mult    = 39U;
        break;
      case PLL252:
        pll_prdiv   = 0U;       //63*4=252
        pll_mult    = 47U;
        break;
      /***********************************************/

      default:
        pll_prdiv   = 0U;
        pll_mult    = 24U;
        break;                  //(unknown value, default initialize 180M)
      }
    }

    core_clk_M = (8 * ((pll_mult + 16)/2/(pll_prdiv + 1)));
    bus_clk_M =core_clk_M/2;
    //  WDOG_Disable();
    //OSC_OUT = 8MHz, 8MHz EXTAL
    SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1)                // System OSC DIV1=1
                  |SCG_SOSCDIV_SOSCDIV2(1);               // System OSC DIV2=1
    SCG->SOSCCFG = SCG_SOSCCFG_RANGE(3)                   // Range=3: high freq (SOSC betw 8MHz-32MHz)
                  |SCG_SOSCCFG_EREFS(1);                  // EREFS=1: Input is external XTAL
    while(SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK);            // Wait for SOSCCSR unlocked state
    SCG->SOSCCSR=SCG_SOSCCSR_SOSCEN_MASK;                 // Enable OSC
    while(!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK));    // Wait for OSC enabling and valid output
    // SPLL Configuration
    // PLL_OUT = 112MHz, 8MHz EXTAL
    SCG->SPLLDIV = SCG_SPLLDIV_SPLLDIV1(1)                // PLL DIV1=1
                 | SCG_SPLLDIV_SPLLDIV2(1);               // PLL DIV2=1

    /* PREDIV=0: Divide SOSC_CLK by 0+1=1 */
    /* MULT=24:  Multiply sys pll by 24+16=40 */
    /* SPLL_CLK = 8MHz / 1 * 40 / 2 = 160 MHz */
    SCG->SPLLCFG = SCG_SPLLCFG_PREDIV(pll_prdiv)          // PLL PREDIV=0
                 | SCG_SPLLCFG_MULT(pll_mult);            // PLL MULT=24

    while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);            // Wait for SPLLCSR unlocked state
    SCG->SPLLCSR=SCG_SPLLCSR_SPLLEN_MASK;                 // Enable PLL
    while(!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK));    // Wait for PLL enabling and valid output
    // HCCR Configuration
    // PLL_OUT = 160MHz, 8MHz EXTAL
    SCG->HCCR = SCG_HCCR_SCS(6)                           // PLL source clock
              | SCG_HCCR_DIVCORE(1)                       // DIVCORE=1, Core
              | SCG_HCCR_DIVBUS(1)                        // DIVBUS=1,  BUS
              | SCG_HCCR_DIVSLOW(2);                      // DIVSLOW=3, Flash
    SMC->PMPROT=SMC_PMPROT_AHSRUN_MASK                    // Allows High Speed Run
              | SMC_PMPROT_AVLP_MASK;                     // Allows Very Low Power Modes
    SMC->PMCTRL=SMC_PMCTRL_RUNM(3);                       // Entry to High Speed Run
    // Wait for High Speed Run mode
}


void SCG_Init(void)
{
  /* Enable clock for PORTX */
  PCC->PCCn[PCC_PORTA_INDEX] = PCC_PCCn_CGC_MASK;
  PCC->PCCn[PCC_PORTB_INDEX] = PCC_PCCn_CGC_MASK;
  PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK;
  PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;
  PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK;

  // *** System OSC ***
  SCG->SOSCDIV=0x00000101;  /* SOSCDIV1 & SOSCDIV2 =1: divide by 1 */
  SCG->SOSCCFG=0x00000034;  /* Range=3: high freq (SOSC betw 8MHz-32MHz)*/
                            /* HGO=0:   Config xtal osc for low power */
                            /* EREFS=1: Input is external XTAL */
  while(SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK); /* Ensure SOSCCSR unlocked */
  SCG->SOSCCSR=0x00000001;  /* LK=0:          SOSCCSR can be written */
                            /* SOSCCMRE=0:    OSC CLK monitor IRQ if enabled */
                            /* SOSCCM=0:      OSC CLK monitor disabled */
                            /* SOSCERCLKEN=0: Sys OSC 3V ERCLK output clk disabled */
                            /* SOSCLPEN=0:    Sys OSC disabled in VLP modes */
                            /* SOSCSTEN=0:    Sys OSC disabled in Stop modes */
                            /* SOSCEN=1:      Enable oscillator */
  while(!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK)); /* Wait for sys OSC clk valid */

  // *** System PLL ***
  SCG->SPLLDIV = 0x00000101;  /* SOSCPLL1 & SPLLDIV2 =1: divide by 1 */
  SCG->SPLLCFG = 0x00180000;  /* PREDIV=0: Divide SOSC_CLK by 0+1=1 */
                              /* MULT=24:  Multiply sys pll by 24+16=40 */
                              /* SPLL_CLK = 8MHz / 1 * 40 / 2 = 160 MHz */
  while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK); /* Ensure SPLLCSR unlocked */
  SCG->SPLLCSR = 0x00000001; /* LK=0:        SPLLCSR can be written */
                             /* SPLLCMRE=0:  SPLL CLK monitor IRQ if enabled */
                             /* SPLLCM=0:    SPLL CLK monitor disabled */
                             /* SPLLSTEN=0:  SPLL disabled in Stop modes */
                             /* SPLLEN=1:    Enable SPLL */
  while(!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)); /* Wait for SPLL valid */

  // *** MODE CONFIG ***
  SCG->RCCR=SCG_RCCR_SCS(6)  /* PLL as clock source*/
        |SCG_RCCR_DIVCORE(1)  /* DIVCORE= 2, Core/Sys clock = 80 MHz*/
        |SCG_RCCR_DIVBUS(3)   /* DIVBUS = 4, bus clock = 40 MHz*/
        |SCG_RCCR_DIVSLOW(7); /* DIVSLOW = 8, SCG slow, flash clock= 20 MHz*/

  SMC->PMCTRL  = SMC_PMCTRL_RUNM(0);    //enter RUN
}
