/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING 
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT 
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR 
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE 
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software 
* and to discontinue the availability of this software.  By using this software, 
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2010, 2011 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_intc.c
* Version      : Applilet3 for RL78/G13 V1.03.01 [11 Oct 2011]
* Device(s)    : R5F100EA
* Tool-Chain   : CA78K0R
* Description  : This file implements device driver for INTC module.
* Creation Date: 2/20/2013
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_intc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_INTC_Create
* Description  : This function initializes INTP module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC_Create(void)
{
    PMK0 = 1U;    /* disable INTP0 operation */
    PIF0 = 0U;    /* clear INTP0 interrupt flag */
    PMK1 = 1U;    /* disable INTP1 operation */
    PIF1 = 0U;    /* clear INTP1 interrupt flag */
    PMK2 = 1U;    /* disable INTP2 operation */
    PIF2 = 0U;    /* clear INTP2 interrupt flag */
    PMK3 = 1U;    /* disable INTP3 operation */
    PIF3 = 0U;    /* clear INTP3 interrupt flag */
    PMK4 = 1U;    /* disable INTP4 operation */
    PIF4 = 0U;    /* clear INTP4 interrupt flag */
    PMK5 = 1U;    /* disable INTP5 operation */
    PIF5 = 0U;    /* clear INTP5 interrupt flag */
    /* Set INTP0 low priority */
    PPR10 = 1U;
    PPR00 = 1U;
    /* Set INTP1 low priority */
    PPR11 = 1U;
    PPR01 = 1U;
    /* Set INTP2 low priority */
    PPR12 = 1U;
    PPR02 = 1U;
    /* Set INTP3 low priority */
    PPR13 = 1U;
    PPR03 = 1U;
    EGP0 = _01_INTP0_EDGE_RISING_SEL | _02_INTP1_EDGE_RISING_SEL | _04_INTP2_EDGE_RISING_SEL |
           _08_INTP3_EDGE_RISING_SEL;
    /* Set INTP1 pin */
    PM5 |= 0x01U;
    /* Set INTP2 pin */
    PM5 |= 0x02U;
    /* Set INTP3 pin */
    PM3 |= 0x01U;
}

/***********************************************************************************************************************
* Function Name: R_INTC0_Start
* Description  : This function clears INTP0 interrupt flag and enables interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC0_Start(void)
{
    PIF0 = 0U;    /* clear INTP0 interrupt flag */
    PMK0 = 0U;    /* enable INTP0 interrupt */
}

/***********************************************************************************************************************
* Function Name: R_INTC0_Stop
* Description  : This function disables INTP0 interrupt and clears interrupt flag.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC0_Stop(void)
{
    PMK0 = 1U;    /* disable INTP0 interrupt */
    PIF0 = 0U;    /* clear INTP0 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_INTC1_Start
* Description  : This function clears INTP1 interrupt flag and enables interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC1_Start(void)
{
    PIF1 = 0U;    /* clear INTP1 interrupt flag */
    PMK1 = 0U;    /* enable INTP1 interrupt */
}

/***********************************************************************************************************************
* Function Name: R_INTC1_Stop
* Description  : This function disables INTP1 interrupt and clears interrupt flag.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC1_Stop(void)
{
    PMK1 = 1U;    /* disable INTP1 interrupt */
    PIF1 = 0U;    /* clear INTP1 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_INTC2_Start
* Description  : This function clears INTP2 interrupt flag and enables interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC2_Start(void)
{
    PIF2 = 0U;    /* clear INTP2 interrupt flag */
    PMK2 = 0U;    /* enable INTP2 interrupt */
}

/***********************************************************************************************************************
* Function Name: R_INTC2_Stop
* Description  : This function disables INTP2 interrupt and clears interrupt flag.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC2_Stop(void)
{
    PMK2 = 1U;    /* disable INTP2 interrupt */
    PIF2 = 0U;    /* clear INTP2 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_INTC3_Start
* Description  : This function clears INTP3 interrupt flag and enables interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC3_Start(void)
{
    PIF3 = 0U;    /* clear INTP3 interrupt flag */
    PMK3 = 0U;    /* enable INTP3 interrupt */
}

/***********************************************************************************************************************
* Function Name: R_INTC3_Stop
* Description  : This function disables INTP3 interrupt and clears interrupt flag.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC3_Stop(void)
{
    PMK3 = 1U;    /* disable INTP3 interrupt */
    PIF3 = 0U;    /* clear INTP3 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_KEY_Create
* Description  : This function initializes the key return module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_KEY_Create(void)
{
    volatile uint8_t  w_count;

    KRMK = 1U;  /* disable INTKR operation */
    KRIF = 0U;  /* clear INTKR interrupt flag */
    /* Set INTKR low priority */
    KRPR1 = 1U;
    KRPR0 = 1U;
    KRM = _01_KR0_SIGNAL_DETECT_ON | _02_KR1_SIGNAL_DETECT_ON | _04_KR2_SIGNAL_DETECT_ON | _08_KR3_SIGNAL_DETECT_ON;
    /* Set KR0 pin */
    PM7 |= 0x01U;
    /* Set KR1 pin */
    PM7 |= 0x02U;
    /* Set KR2 pin */
    PM7 |= 0x04U;
    /* Set KR3 pin */
    PM7 |= 0x08U;

    /* Wait 250ns */
    for (w_count = 0U; w_count < KEY_WAITTIME; w_count++)
    {   
        NOP();
    }
}

/***********************************************************************************************************************
* Function Name: R_KEY_Start
* Description  : This function clears INTKR interrupt flag and enables interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_KEY_Start(void)
{
    KRIF = 0U;  /* clear INTKR interrupt flag */
    KRMK = 0U;  /* enable INTKR operation */
}

/***********************************************************************************************************************
* Function Name: R_KEY_Stop
* Description  : This function disables INTKR interrupt and clears interrupt flag.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_KEY_Stop(void)
{
    KRMK = 1U;  /* disable INTKR operation */
    KRIF = 0U;  /* clear INTKR interrupt flag */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
