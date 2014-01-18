/*************************************************************************************
**																					**
**	microapi.c			RL78 - R5F100EEANA Processor     							**
** 																					**
**************************************************************************************
**																					**
** Written By:	Steve Montgomery													**
**				Digital Six Laboratories LLC										**
** (c)2012 Digital Six Labs, All rights reserved									**
**																					**
**************************************************************************************/
//
// Revision History
//
// Revision		Date	Revisor		Description
// ===================================================================================
// ===================================================================================
#include "microapi.h"

// Global Variables
U32 xdata sysclk;
U32 xdata rtccapture;
struct TimeOfDay xdata rtc;
#ifdef UART_ENABLED
extern U8 _uart0ReceiveBuffer[UARTBUFFERSIZE];
extern U8 _uart0ReceiveTailPointer;
extern U8 _uart1ReceiveBuffer[UARTBUFFERSIZE];
extern U8 _uart1ReceiveTailPointer;
extern U8 _uart0TransmitBuffer[UARTBUFFERSIZE];
extern U8 _uart0TransmitTailPointer;
extern U8 _uart1TransmitBuffer[UARTBUFFERSIZE];
extern U8 _uart1TransmitTailPointer;
U8 _uart0TransmitHeadPointer;
U8 _uart0ReceiveHeadPointer;
U8 _uart1TransmitHeadPointer;
U8 _uart1ReceiveHeadPointer;
#endif
struct
{

	U8 BaudRate;
	U8 ResetReason;
} microPrivateData;


// ***********************************************************************************
// *** Interrupt Handlers ***

// ***********************************************************************************
// *** Internal functions ***
void InitializeTimers()
{
	RTCEN = 1U;
	RTCE = 0;
	// setup interval timer for 1 mSec interrupts
	ITMC = 0;
	ITMK = 1;
	ITIF = 0;
	ITPR1 = 1;
	ITPR0 = 1;
	ITMC = 0x2f;

	// Setup real time clock
    RTCE = 0U;     /* disable RTC clock operation */
    RTCMK = 1U;    /* disable INTRTC interrupt */
    RTCIF = 0U;    /* clear INTRTC interrupt flag */
    /* Set INTRTC high priority */
    RTCPR1 = 0U;
    RTCPR0 = 0U;
    RTCC0 = 0x01;
}
void InitializePorts()
{
	// P10-P12 are used for SPI for RF Module.  P13 is input.  P14 is output.  The rest are inputs.
    PM1 = _01_PMn0_NOT_USE | _02_PMn1_NOT_USE | _04_PMn2_NOT_USE | _08_PMn3_MODE_INPUT | _00_PMn4_MODE_OUTPUT |
          _20_PMn5_NOT_USE | _40_PMn6_MODE_INPUT | _80_PMn7_MODE_INPUT;
    /* Port 2 Configuration */
    // AD0-AD5 are analog (P20-P25) P26 is digital input
    ADPC = _07_ADPC_DI_ON;
    PM2 = 0xFF;
    /* Port 3 Configuration */
    // P30-P31 are digital inputs
    PM3 = 0xff;

    PM5 = 0xff;

    /* Port 6 Configuration */
    // P60-61 are used for IIC, P62 is an input.
    PM6 = 0xff;
    /* Port 7 Configuration */
    // P70-73 are digital inputs
    PM7 = 0xff;
    /* Port 12 configuration */
    // P120 is an output and P121-P122 are inputs
    PM12 = 0xfe;
    PMC12 = 0x00;

    /* Port 13 configuration */
    // On the R5F100EEANA, Port 13 pins are hard configured.  No configuration from firmware is possible

    /* Port 14 Configuration */
    // P147 is an input
    PM14 = 0xff;
}
// Sub clock is on and uses external oscillator
// Main clock is on and uses internal high speed oscillator
// RTC is driven by sub clock
void InitializeClock()
{
    volatile U16 w_count;

    /* Set fMX */
    // enable the 32khz oscillator
    CMC = _00_CGC_HISYS_PORT | _10_CGC_SUB_OSC | _00_CGC_SYSOSC_DEFAULT | _00_CGC_SUBMODE_LOW;
    // make sure the external high speed oscillator is stopped
    MSTOP = 1;
    /* Set fMAIN */
    // fmain is driven by internal high speed oscillator
    MCM0 = 0;
    /* Set fSUB */
    // sub osc is operating
    XTSTOP = 0;

    OSMC = _00_CGC_SUBINHALT_ON | _00_CGC_RTC_CLK_FSUB;
    /* Set fCLK */
    // make sure we are being clocked by the main oscillator and not the sub oscillator
    CSS = 0;
    /* Set fIH */
    // make sure on chip oscillator is running
    HIOSTOP = 0;
}
void InitializeSPI()
{
	U8 regVal;
    SAU0EN = 1U;    /* supply SAU0 clock */
    NOP();
    NOP();
    NOP();
    NOP();
    SPS0 = _0001_SAU_CK00_FCLK_1  |  _0010_SAU_CK01_FCLK_1;
	// ** SPI 0 Setup **
    CSIMK00 = 1U;    /* disable INTCSI00 interrupt */
    CSIIF00 = 0U;    /* clear INTCSI00 interrupt flag */
    /* Set INTCSI00 low priority */
    CSIPR100 = 1U;
    CSIPR000 = 1U;
	// Clear SE1
	ST0 |= 0x01;
	// SPI with interrupt on the end of the transfer
	SMR00 = 0x0020U;
	// Tx/Rx, normal polarity, no parity, MSB first, no stop bit, 8 bit data length
	SCR00 = 0xC007;//C000_SAU_RECEPTION_TRANSMISSION | _3000_SAU_TIMING_4 | _0000_SAU_MSB | _0007_SAU_LENGTH_8;
	// Fmck/2 divisor
	SDR00 = 0x0U;
	// Clear all of the flags
	SIR00 = 0x0007U;
	// SCK0 and SO0 are outputs
	SO0 &= ~_0100_SAU_CH0_CLOCK_OUTPUT_1;    /* CSI00 clock initial level */
	SO0 &= ~_0001_SAU_CH0_DATA_OUTPUT_1;    /* CSI00 SO initial level */

	SOE0 |= _0001_SAU_CH0_OUTPUT_ENABLE;    /* enable CSI00 output */

	// Set SI00 pin
	PM1 |= 0x02U;
	// Set SO00 pin
	P1 |= 0x04U;
	PM1 &= 0xFBU;
	// Set SCK00 pin
	P1 |= 0x01U;
	PM1 &= 0xFEU;
	regVal = PM1;
	NSSpin = HIGH;
}
void EnableSPI()
{
    CSIIF00 = 0U;    /* clear INTCSI00 interrupt flag */
    //CSIMK00 = 0U;    /* enable INTCSI00 */
    SO0 &= ~_0100_SAU_CH0_CLOCK_OUTPUT_1;    /* CSI00 clock initial level */
    SO0 &= ~_0001_SAU_CH0_DATA_OUTPUT_1;           /* CSI00 SO initial level */
    SOE0 |= _0001_SAU_CH0_OUTPUT_ENABLE;           /* enable CSI00 output */
    SS0 |= _0001_SAU_CH0_START_TRG_ON;             /* enable CSI00 */
}
void DisableSPI()
{
	ST0 |= 0x01;
	SOE1 &= ~0x01;
	CSIIF00 = 0;
}

void InitializeSPI2()
{
	U8 regVal;
    SAU1EN = 1U;    /* supply SAU0 clock */
    NOP();
    NOP();
    NOP();
    NOP();
    SPS1 = 00;;
	// ** SPI 0 Setup **
    CSIMK20 = 1U;    /* disable INTCSI00 interrupt */
    CSIIF20 = 0U;    /* clear INTCSI00 interrupt flag */
    /* Set INTCSI00 low priority */
    CSIPR120 = 1U;
    CSIPR020 = 1U;
	// Clear SE1
	ST0 |= 0x01;
	// SPI with interrupt on the end of the transfer
	SMR10 = 0x8020U;
	// Tx/Rx, normal polarity, no parity, MSB first, no stop bit, 8 bit data length
	SCR10 = 0xC007;//C000_SAU_RECEPTION_TRANSMISSION | _3000_SAU_TIMING_4 | _0000_SAU_MSB | _0007_SAU_LENGTH_8;
	// Fmck/210 divisor
	SDR10 = 0x2000U;
	// Clear all of the flags
	SIR10 = 0x0007U;
	// SCK0 and SO0 are outputs
	SO1 &= ~_0000_SAU_CH0_CLOCK_OUTPUT_0;    /* CSI00 clock initial level */
	SO1 &= ~_0000_SAU_CH0_DATA_OUTPUT_0;    /* CSI00 SO initial level */

	SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;    /* enable CSI00 output */

	// Set SI00 pin
	PM1 |= 0x10U;
	// Set SO00 pin
	P1 |= 0x08U;
	PM1 &= 0xF7U;
	// Set SCK00 pin
	P1 |= 0x20U;
	PM1 &= 0xDFU;
	regVal = PM1;
	NSS2pin = HIGH;
}
void EnableSPI2()
{
    CSIIF20 = 0U;    /* clear INTCSI00 interrupt flag */
    //CSIMK00 = 0U;    /* enable INTCSI00 */
    SO1 &= ~_0100_SAU_CH0_CLOCK_OUTPUT_1;    /* CSI00 clock initial level */
    SO1 &= ~_0001_SAU_CH0_DATA_OUTPUT_1;           /* CSI00 SO initial level */
    SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;           /* enable CSI00 output */
    SS1 |= _0001_SAU_CH0_START_TRG_ON;             /* enable CSI00 */
}
void DisableSPI2()
{
	ST1 |= 0x01;
	SOE1 &= ~0x01;
	CSIIF20 = 0;
}
void InitializeIIC()
{
    IICA0EN = 1U; /* supply IICA0 clock */
    IICE0 = 0U; /* disable IICA0 operation */
    IICAMK0 = 1U; /* disable INTIICA0 interrupt */
    IICAIF0 = 0U; /* clear INTIICA0 interrupt flag */
    /* Set INTIICA0 low priority */
    IICAPR10 = 1U;
    IICAPR00 = 1U;
    /* Set SCLA0, SDAA0 pin */
    P6 &= 0xFCU;
    PM6 |= 0x03U;
    // Low speed
    SMC0 = 0U;

    // 100khz clock rate
    IICWL0 = _4C_IICA0_IICWL_VALUE;
    IICWH0 = _55_IICA0_IICWH_VALUE;
    IICCTL01 |= _01_IICA_fCLK_HALF;
    SVA0 = _10_IICA0_MASTERADDRESS;
    STCEN0 = 1U;
    IICRSV0 = 1U;
    SPIE0 = 0U;
    WTIM0 = 1U;
    ACKE0 = 1U;
    IICAMK0 = 0U;
    IICE0 = 1U;
    LREL0 = 1U;
    /* Set SCLA0, SDAA0 pin */
    PM6 &= 0xFCU;
}
void InitializeInterrupts()
{
	volatile U8  w_count;

	KRMK = 1U;  /* disable INTKR operation */
	KRIF = 0U;  /* clear INTKR interrupt flag */
	/* Set INTKR low priority */
	KRPR1 = 1U;
	KRPR0 = 1U;

	/* Wait 250ns */
	for (w_count = 0U; w_count < KEY_WAITTIME; w_count++)
	{
		//NOP();
	}
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
    // Set INTP0 pin

    /* Set INTP1 pin */
    PM5 |= 0x01U;
    /* Set INTP2 pin */
    PM5 |= 0x02U;
    /* Set INTP3 pin */
    PM3 |= 0x01U;
    // disable RTC constant period interrupt
    RTCMK = 1;
    RTCIF = 0;

}
void InitializeUART0()
{
#ifdef UART_ENABLED
	_uart0ReceiveTailPointer = 0;
	_uart0TransmitTailPointer = 0;
	_uart0ReceiveHeadPointer = 0;
	_uart0TransmitHeadPointer = 0;

    ST0 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;    /* disable UART0 receive and transmit */
    STMK0 = 1U;    /* disable INTST0 interrupt */
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    SRMK0 = 1U;    /* disable INTSR0 interrupt */
    SRIF0 = 0U;    /* clear INTSR0 interrupt flag */
    SREMK0 = 1U;   /* disable INTSRE0 interrupt */
    SREIF0 = 0U;   /* clear INTSRE0 interrupt flag */
    /* Set INTST0 low priority */
    STPR10 = 1U;
    STPR00 = 1U;
    /* Set INTSR0 low priority */
    SRPR10 = 1U;
    SRPR00 = 1U;
    /* Set INTSRE0 low priority */
    SREPR10 = 1U;
    SREPR00 = 1U;
    SMR00 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE |
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR00 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 |
            _0007_SAU_LENGTH_8;
    SDR00 = _CE00_UART0_TRANSMIT_DIVISOR;
    NFEN0 |= _01_SAU_RXD0_FILTER_ON;
    SIR01 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR01 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL |
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR01 = _4000_SAU_RECEPTION | _0400_SAU_INTSRE_ENABLE | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 |
            _0007_SAU_LENGTH_8;
    SDR01 = _CE00_UART0_RECEIVE_DIVISOR;
    SO0 |= _0001_SAU_CH0_DATA_OUTPUT_1;
    SOL0 |= _0000_SAU_CHANNEL0_NORMAL;    /* output level normal */
    SOE0 |= _0001_SAU_CH0_OUTPUT_ENABLE;    /* enable UART0 output */
    /* Set RxD0 pin */
    PM1 |= 0x02U;
    /* Set TxD0 pin */
    P1 |= 0x04U;
    PM1 &= 0xFBU;
#endif
}

void InitializeUART1(tBaudRates baudRate)
{
#ifdef UART_ENABLED
	_uart1ReceiveTailPointer = 0;
	_uart1TransmitTailPointer = 0;
	_uart1ReceiveHeadPointer = 0;
	_uart1TransmitHeadPointer = 0;

    ST0 |= _0008_SAU_CH3_STOP_TRG_ON | _0004_SAU_CH2_STOP_TRG_ON;    /* disable UART1 receive and transmit */
    STMK1 = 1U;    /* disable INTST1 interrupt */
    STIF1 = 0U;    /* clear INTST1 interrupt flag */
    SRMK1 = 1U;    /* disable INTSR1 interrupt */
    SRIF1 = 0U;    /* clear INTSR1 interrupt flag */
    SREMK1 = 1U;   /* disable INTSRE1 interrupt */
    SREIF1 = 0U;   /* clear INTSRE1 interrupt flag */
    /* Set INTST1 low priority */
    STPR11 = 1U;
    STPR01 = 1U;
    /* Set INTSR1 low priority */
    SRPR11 = 1U;
    SRPR01 = 1U;
    /* Set INTSRE1 low priority */
    SREPR11 = 1U;
    SREPR01 = 1U;
    SMR02 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE |
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR02 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 |
            _0007_SAU_LENGTH_8;
    SDR02 = _CE00_UART1_TRANSMIT_DIVISOR;
    NFEN0 |= _04_SAU_RXD1_FILTER_ON;
    SIR03 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR03 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL |
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR03 = _4000_SAU_RECEPTION | _0400_SAU_INTSRE_ENABLE | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 |
            _0007_SAU_LENGTH_8;
    SDR03 = _CE00_UART1_RECEIVE_DIVISOR;

    SetUART1BaudRate(baudRate);
    SO0 |= _0004_SAU_CH2_DATA_OUTPUT_1;
    SOL0 |= _0000_SAU_CHANNEL2_NORMAL;    /* output level normal */
    SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;  /* enable UART1 output */
    /* Set RxD1 pin */
    PM0 |= 0x02U;
    /* Set TxD1 pin */
    P0 |= 0x01U;
    PM0 &= 0xFEU;


    STIF1 = 0U;    /* clear INTST1 interrupt flag */
    STMK1 = 0U;    /* enable INTST1 interrupt */
    SRIF1 = 0U;    /* clear INTSR1 interrupt flag */
    SRMK1 = 0U;    /* enable INTSR1 interrupt */
    SREIF1 = 0U;   /* clear INTSRE1 interrupt flag */
    SREMK1 = 0U;   /* enable INTSRE1 interrupt */
    SO0 |= _0004_SAU_CH2_DATA_OUTPUT_1;    /* output level normal */
    SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;    /* enable UART1 output */
    SS0 |= _0008_SAU_CH3_START_TRG_ON | _0004_SAU_CH2_START_TRG_ON;    /* enable UART1 receive and transmit */
#endif
}
// ***********************************************************************************
// *** Public API ***

U8 InitializeMicroAPI(void)
{
	U8 resetSource;
	pfdl_descriptor_t pfdl_desc;


	// PIOR0 and PIOR1 and PIOR2 is set to 0.  This configures the IO as follows:
	// SCLA0 - P60
	// SDAA0 - P61
	// TXD2 - P13
	// RXD2 - P14
	// SCL20 - P15
	// SDA20 - P14
	// SI20 - P14
	// SO20 - P13
	// SCK20 - P15
	// TXD0 - P12
	// RXD0 - P11
	// SCL0 - P10
	// SDA00 - P11
	// SI00 - P11
	// SO00 - P12
	// SCK00 - P10
	// TI02/TO02 - P17
	// TI03/TO03 - P31
	PIOR = 0x00;

	resetSource = RESF;

	InitializePorts();
	InitializeClock();
	InitializeSPI();
	//InitializeSPI2();
	/*
	InitializeIIC();
	InitializeInterrupts();
	InitializeUART0();
	InitializeUART1();
	*/
	InitializeUART1(k115200);
	InitializeInterrupts();
	NSSpin = LOW;
	NSSpin = HIGH;

	InitializeTimers();
	SetCPI(0x02); // set constant period interrupt for 1 sec
	SetSubOscillatorMode(0);
	SubOscillatorOn();
	RealTimeClockOn();
	StartIntervalTimer();

	pfdl_desc.fx_MHz_u08 = 8;
	pfdl_desc.wide_voltage_mode_u08=0x01;

	PFDL_Open(&pfdl_desc);


	return resetSource;

}

// *****************************************************************************
// ** SPI

U8 ReadCharSPI(U8 reg)
{
   U8 value;
   U8 waitTime = 0xff;
   // make sure MSB is low for read
   reg&=0x7f;
   NSSpin = LOW;
   EnableSPI();
   SIO00 = reg;
   while(!CSIIF00)
   {
	   waitTime--;
	   if(waitTime==0)
		   return 0;
   }
   DisableSPI();
   EnableSPI();
   SIO00 = 0x55;
   waitTime = 0xff;
   while(!CSIIF00)
   {
	   waitTime--;
	   if(waitTime==0)
		   return 0;
   }
   DisableSPI();
   value = SIO00;
   NSSpin = HIGH;
   return value;
}
void WriteCharSPI(U8 reg, U8 value)
{
   U16 waitTime = 0xffff;

   // make sure MSB is high for write
   reg|=0x80;
   NSSpin = LOW;
   EnableSPI();
   SIO00 = reg;
   while(!CSIIF00)
   {
	   waitTime--;
	   if(waitTime==0)
		   return;
   }
   DisableSPI();
   EnableSPI();
   SIO00 = value;
   waitTime = 0xffff;
   while(!CSIIF00)
   {
	   waitTime--;
	   if(waitTime==0)
		   return;
   }
   DisableSPI();
   NSSpin = HIGH;
}
void WriteCharSPIMultiple (U8 count, U8 reg,  U8 *addr)
{
   U8 waitTime = 0xff;
   // make sure MSB is high for write
   reg|=0x80;
   NSSpin = LOW;
   SIO00 = reg;
   while(!CSIIF00)
   {
	   while(waitTime--)
		   return;
   }
   CSIIF00 = 0U;    /* clear INTCSI00 interrupt flag */
   while(count--)
   {
	   SIO00 = *(addr++);
	   waitTime = 0xff;
	   while(!CSIIF00)
	   {
		   while(waitTime--)
			   return;
	   }
	    CSIIF00 = 0U;    /* clear INTCSI00 interrupt flag */
   }
   NSSpin = HIGH;
}
void ReadCharSPIMultiple(U8 address, U8 count, U8 *receiveBuffer)
{
	U8 waitTime = 0xff;
	// make sure MSB is low for read
	address&=0x7f;
	NSSpin = LOW;
	SIO00 = address;
	while(!CSIIF00)
	{
	   while(waitTime--)
		   return;
	}
    CSIIF00 = 0U;    /* clear INTCSI00 interrupt flag */
	while(count--)
	{
		SIO00 = 0x55;
		waitTime = 0xff;
		while(!CSIIF00)
		{
		   while(waitTime--)
			   return;
		}
	    CSIIF00 = 0U;    /* clear INTCSI00 interrupt flag */
		*(receiveBuffer++) = SIO20;
	}
	NSSpin = HIGH;
}

// *****************************************************************************
// ** SPI2

U8 ReadCharSPI2(U8 reg)
{
   U8 value;
   U8 waitTime = 0xff;
   // make sure MSB is low for read
   reg&=0x7f;
   NSS2pin = LOW;
   EnableSPI2();
   SIO20 = reg;
   while(!CSIIF20)
   {
	   waitTime--;
	   if(waitTime==0)
		   return 0;
   }
   DisableSPI2();
   EnableSPI2();
   SIO20 = 0x55;
   waitTime = 0xff;
   while(!CSIIF20)
   {
	   waitTime--;
	   if(waitTime==0)
		   return 0;
   }
   DisableSPI2();
   value = SIO20;
   NSS2pin = HIGH;
   return value;
}
void WriteCharSPI2(U8 reg, U8 value)
{
   U16 waitTime = 0xffff;
   U8 retVal;
   U8 regVal;

   regVal = reg;

   // make sure MSB is high for write
   reg|=0x80;
   NSS2pin = LOW;
   EnableSPI2();
   SIO20 = reg;
   while(!CSIIF20)
   {
	   waitTime--;
	   if(waitTime==0)
		   return;
   }
   DisableSPI2();
   EnableSPI2();
   SIO20 = value;
   waitTime = 0xffff;
   while(!CSIIF20)
   {
	   waitTime--;
	   if(waitTime==0)
		   return;
   }
   DisableSPI2();
   NSS2pin = HIGH;

   // NOTE!!!!!!!
   // Put nothing else here.  No checks, no extra code, nothing.  It will interfere with the modes.
   return;
}
void WriteCharSPI2Multiple (U8 count, U8 reg,  U8 *addr)
{
   U8 waitTime = 0xff;
   // make sure MSB is high for write
   reg|=0x80;
   NSSpin = LOW;
   SIO20 = reg;
   while(!CSIIF20)
   {
	   while(waitTime--)
		   return;
   }
   CSIIF20 = 0U;    /* clear INTCSI00 interrupt flag */
   while(count--)
   {
	   SIO20 = *(addr++);
	   waitTime = 0xff;
	   while(!CSIIF20)
	   {
		   while(waitTime--)
			   return;
	   }
	    CSIIF20 = 0U;    /* clear INTCSI00 interrupt flag */
   }
   NSSpin = HIGH;
   return;
}
void ReadSPI2Multiple(U8 address, U8 count, U8 *receiveBuffer)
{
	U8 waitTime = 0xff;
	// make sure MSB is low for read
	address&=0x7f;
	NSSpin = LOW;
	SIO20 = address;
	while(!CSIIF20)
	{
	   while(waitTime--)
		   return;
	}
    CSIIF20 = 0U;    /* clear INTCSI00 interrupt flag */
	while(count--)
	{
		SIO20 = 0x55;
		waitTime = 0xff;
		while(!CSIIF20)
		{
		   while(waitTime--)
			   return;
		}
	    CSIIF20 = 0U;    /* clear INTCSI00 interrupt flag */
		*(receiveBuffer++) = SIO20;
	}
	NSSpin = HIGH;
	return;
}
// *****************************************************************************
// ** UART0
U8 ReadCharUART0(void)
{
#ifdef UART_ENABLED
	U8 returnValue = 0;
	DI();
	if(_uart0ReceiveTailPointer!=_uart0ReceiveHeadPointer)
	{
		returnValue = _uart0ReceiveBuffer[_uart0ReceiveHeadPointer++];
		_uart0ReceiveHeadPointer &= UARTBUFFERSIZE-1;
	}
	EI();
	return returnValue;
#endif
}
void WriteCharUART0(U8 charToWrite)
{
#ifdef UART_ENABLED
	DI();
	_uart0TransmitBuffer[_uart0TransmitHeadPointer++] = charToWrite;
	_uart0TransmitHeadPointer &= UARTBUFFERSIZE - 1;
	EI();
#endif
}
U8 BufferCountUART0(void)
{

	U8 retVal;
#ifdef UART_ENABLED
	DI();
	retVal = _uart0ReceiveHeadPointer - _uart0ReceiveTailPointer;
	retVal &= UARTBUFFERSIZE-1;
	EI();
#endif
	return retVal;

}
U8 Uart0PeekByte()
{
	U8 retVal=0;
#ifdef UART_ENABLED
	DisableInterrupts;
	if(_uart0ReceiveHeadPointer != _uart0ReceiveTailPointer)
		retVal = _uart0ReceiveBuffer[_uart0ReceiveTailPointer];
	_uart0ReceiveTailPointer &= UARTBUFFERSIZE-1;
	EnableInterrupts;
#endif
	return retVal;
}
void Uart0PeekBytes(U8 count, U8 *buffer)
{
#ifdef UART_ENABLED
	int i, oldTail;

	DisableInterrupts;
	oldTail =_uart0ReceiveTailPointer;
	if(count<BufferCountUART0())
	{
		for(i=0;i<count;i++)
		{
			*buffer =_uart0ReceiveBuffer[_uart0ReceiveTailPointer++];
			buffer++;
		}
	}
	_uart0ReceiveTailPointer = oldTail;
	EnableInterrupts;
#endif
}
// *****************************************************************************
// ** UART 1

U8 ReadCharUART1(void)
{
	U8 returnValue = 0;
#ifdef UART_ENABLED
	DI();
	if(_uart1ReceiveTailPointer!=_uart1ReceiveHeadPointer)
	{
		returnValue = _uart1ReceiveBuffer[_uart1ReceiveHeadPointer++];
		_uart1ReceiveHeadPointer &= UARTBUFFERSIZE-1;
		if(_uart1ReceiveHeadPointer>UARTBUFFERSIZE)
			_uart1ReceiveHeadPointer=0;
	}
	EI();
#endif
	return returnValue;
}
void WriteCharUART1(U8 charToWrite)
{
#ifdef UART_ENABLED
	DI();
	_uart1TransmitBuffer[_uart1TransmitHeadPointer] = charToWrite;
	if(_uart1TransmitHeadPointer == _uart1TransmitTailPointer)
	{
	    STIF1 = 0U;    /* clear INTST1 interrupt flag */
	    STMK1 = 0U;    /* enable INTST1 interrupt */
	    TXD1=charToWrite;
	}
	_uart1TransmitHeadPointer++;
	_uart1TransmitHeadPointer &= UARTBUFFERSIZE - 1;
	EI();
#endif
}
U8 BufferCountUART1(void)
{
	U8 retVal;
#ifdef UART_ENABLED
	DI();
	retVal = _uart1ReceiveTailPointer - _uart1ReceiveHeadPointer;
	if(retVal!=0)
		retVal &= UARTBUFFERSIZE-1;
	EI();
#endif
	return retVal;
}
U8 Uart1PeekByte()
{
	U8 retVal=0;
#ifdef UART_ENABLED
	DisableInterrupts;
	if(_uart1ReceiveHeadPointer != _uart1ReceiveTailPointer)
		retVal = _uart1ReceiveBuffer[_uart1ReceiveHeadPointer];
	_uart1ReceiveTailPointer &= UARTBUFFERSIZE-1;
	EnableInterrupts;
#endif
	return retVal;
}
void Uart1PeekBytes(U8 count, U8 *buffer)
{
#ifdef UART_ENABLED
	int i, oldTail;

	DisableInterrupts;
	oldTail =_uart1ReceiveTailPointer;
	if(count<BufferCountUART1())
	{
		for(i=0;i<count;i++)
		{
			*buffer =_uart1ReceiveBuffer[_uart1ReceiveTailPointer++];
			buffer++;
		}
	}
	_uart1ReceiveTailPointer = oldTail;
	EnableInterrupts;
#endif
}
void SetUART1BaudRate(tBaudRates baudRate)
{
#ifdef UART_ENABLED
	microPrivateData.BaudRate = baudRate;
    SS0 &= ~(_0008_SAU_CH3_START_TRG_ON | _0004_SAU_CH2_START_TRG_ON);    /* disable UART1 receive and transmit */
	switch(baudRate)
	{
	case k76800:
		SDR02 = 0xCF00;
		SDR03 = 0xCF00;
		break;
	case k115200:
		SDR02 = 0x8900;
		SDR03 = 0x8900;
		break;
	}

    SS0 |= _0008_SAU_CH3_START_TRG_ON | _0004_SAU_CH2_START_TRG_ON;    /* enable UART1 receive and transmit */
#endif
}
U8 GetUART1BaudRate()
{
#ifdef UART_ENABLED
	return microPrivateData.BaudRate;
#endif
}
// *****************************************************************************
// **  I2C

U8 ReadCharIIC(U8 address)
{
	U8 waitTime = 255;
	// Set start condition
	STT0 = 1;
	// wait for start condition
	while(waitTime--)
	{
		;
	}
	// send address
	IICA0 = (address<<1) & 0xfe;
	// wait for ack
	waitTime = 255;
	while(!ACKD0)
	{
		waitTime--;
		if(waitTime==0)
			return -4;
	}
	// clear the ISF
	IICAIF0 = 0;
	ACKE0 = 1;
	WTIM0 = 0;
	WREL0 = 1;
	// wait for transfer to end
	waitTime = 255;
	while(!IICAIF0)
	{
		waitTime--;
		if(waitTime==0)
			return -4;
	}
	return IICA0;
}
U8 ReadMultipleIIC(U8 count, U8 address, U8 *buffer)
{
	U8 waitTime = 255;
	// Set start condition
	STT0 = 1;
	// wait for start condition
	while(waitTime--)
	{
		;
	}
	// send address
	IICA0 = (address<<1) & 0xfe;
	// wait for ack
	waitTime = 255;
	while(!ACKD0)
	{
		waitTime--;
		if(waitTime==0)
			return 1;
	}
	// clear the ISF
	IICAIF0 = 0;
	ACKE0 = 1;
	WTIM0 = 0;
	WREL0 = 1;
	while(count--)
	{
		// wait for transfer to end
		waitTime = 255;
		while(!IICAIF0)
		{
			waitTime--;
			if(waitTime==0)
				return 1;
		}
		*(buffer++)=IICA0;
		// clear the ISF
		IICAIF0 = 0;
	}
	return 0;
}

U8 WriteCharIIC(U8 address, U8 byteToWrite)
{
	U8 waitTime = 255;
	// Set start condition
	STT0 = 1;
	// wait for start condition
	while(waitTime--)
	{
		;
	}
	// send address
	IICA0 = (address<<1) & 0xfe;
	// wait for ack
	waitTime = 255;
	while(!ACKD0)
	{
		waitTime--;
		if(waitTime==0)
			return 1;
	}
	// clear the ISF
	IICAIF0 = 0;
	IICA0 = byteToWrite;
	// wait for final ack
	waitTime = 255;
	while(!ACKD0)
	{
		waitTime--;
		if(waitTime==0)
			return 1;
	}
	// clear the ISF
	IICAIF0 = 0;
	return 0;
}
U8 WriteMultipleIIC(U8 count, U8 address, U8 *buffer)
{
	U8 waitTime = 255;
	// Set start condition
	STT0 = 1;
	// wait for start condition
	while(waitTime--)
	{
		;
	}
	// send address
	IICA0 = (address<<1) & 0xfe;
	while(count>0)
	{
		count--;
		// wait for ack
		waitTime = 255;
		while(!ACKD0)
		{
			waitTime--;
			if(waitTime==0)
				return 1;
		}
		// clear the ISF
		IICAIF0 = 0;
		IICA0 = *(buffer++);
	}
	// wait for final ack
	waitTime = 255;
	while(!ACKD0)
	{
		waitTime--;
		if(waitTime==0)
			return 1;
	}
	return 0;
}
// *****************************************************************************
// ** Analog

void AnanlogConversionOn(void)
{
	ADCE = 1;
}
void AnalogConversionOff(void)
{
	ADCE = 0;
}
void AnalogConversionStart(void)
{
	ADCS = 1;
}
void AnalogConversionStop(void)
{
	ADCS = 0;
}
void AnalogConverterConfigure(tAnalogConfiguration *config)
{
	ADM0 = config->ChannelSelectionMode | config->ConversionRate;
	ADM1 = config->TriggerMode | config->ConversionMode | config->HardwareTrigger;
	ADM2 = config->ReferenceSource | config->LimitCheckMode | config->SnoozeMode;
	ADUL = config->UpperLimit;
	ADLL = config->LowerLimit;
}
U16 AnalogGet10BitResult()
{
	return ADCR;
}
U8 AnalogGet8BitResult()
{
	return ADCRH;
}
void AnalogSetInputChannel(U8 channel)
{
	ADS&=channel;
}
// *****************************************************************************
// ** Interval Timer

void StartIntervalTimer()
{
	// Enable the timer and set interval to 1 msec
	// based on 8mhz
	ITMC = 0x802B;
	// enable the interrupt
	ITIF = 0;
	ITMK = 0;
}
void StopIntervalTimer()
{
	// disable the interrupt
	ITMK = 1;
	ITIF = 0;
	// disable the timer
	ITMC &= 0x8000;
}
// *****************************************************************************
// ** Radio IO

void ResetRadio(void)
{
	// not supported in rfBrick
	/*
	U32 resetCounter = 0;
	RadioResetPin = HIGH;
	while(resetCounter<10000)
		resetCounter++;
	RadioResetPin = LOW;
	resetCounter = 0;
	while(resetCounter<1000000)
		resetCounter++;
		*/
}


void SetIOForReceive()
{
	// enable CRCOK/PKTSENT interrupt
	PMK3 = 0;
}
void SetIOForTransmit()
{

}
// *****************************************************************************
// ** Oscillators

void SetMainClockToSubOscillator(void)
{
	CSS = 1;
}
void InternalHSOscillatorOff(void)
{
	HIOSTOP=1;
}
void InternalHSOscillatorOn(void)
{
	HIOSTOP=0;
}
void SubOscillatorOff(void)
{
	XTSTOP=1;
}
void SubOscillatorOn(void)
{
	XTSTOP=0;
}
void SetSubOscillatorMode(U8 mode)
{
	switch(mode)
	{
	// Low power mode
	case 0:
		AMPHS0 = 0;
		AMPHS1 = 0;
		break;
	// normal mode
	case 1:
		AMPHS0 = 1;
		AMPHS1 = 0;
		break;
	// ultra low power mode
	case 2:
		AMPHS0 = 0;
		AMPHS1 = 1;
		break;
	}
}
// *****************************************************************************
// ** Real time clock

void RealTimeClockOn(void)
{
	RTCE = 1;
	// enable RTC constant period interrupt
	RTCMK = 0;
	RTCIF = 0;
}
void RealTimeClockOff(void)
{
	RTCE = 0;
	// disable RTC constant period interrupt
	RTCMK = 1;
	RTCIF=0;
}
void EnableRTC1HZ(void)
{
	RCLOE1 = 1;
}
void DisableRTC1HZ(void)
{
	RCLOE1 = 0;
}
void SetRTCHourMode(U8 mode)
{
	AMPM = mode;
}
void SetCPI(U8 mode)
{
	RTCMK = 1;
	RTCC0 &= ~0x07;
	RTCC0 |= (mode & 0x07);
	RTCC1 &= ~0x08;
	RTCIF = 0;
	RTCMK =0;
}
void EnableRTCAlarm(U8 enableInterrupt)
{
	WALE = 1;
	WALIE = enableInterrupt;
}
void DisableRTCAlarm(void)
{
	WALE = 0;
	WALIE = 0;
}
// *****************************************************************************
// ** WDT

void HitWDT(void)
{
	WDTE = 0xAC;
}
// *****************************************************************************
// ** Data FLASH

void ErasePersistentArea()
{
	pfdl_status_t ps;
	pfdl_request_t req;


	req.command_enu = PFDL_CMD_ERASE_BLOCK;
	req.index_u16 = 0;

	PFDL_Execute(&req);

	ps = PFDL_Handler();
	while(ps==PFDL_BUSY)
		ps=PFDL_Handler();
}
void WritePersistentValue(U16 address, U8 *value, U8 count)
{
	pfdl_status_t ps;
	pfdl_request_t req;


	req.bytecount_u16=count;
	req.command_enu = PFDL_CMD_WRITE_BYTES;
	req.index_u16 = address;
	req.data_pu08 = value ;

	PFDL_Execute(&req);

	ps = PFDL_Handler();
	while(ps==PFDL_BUSY)
		ps=PFDL_Handler();
}
U8 ReadPersistentValue(U16 address)
{
	pfdl_status_t ps;
	pfdl_request_t req;
	U8 db;

	req.bytecount_u16=1;
	req.command_enu = PFDL_CMD_READ_BYTES;
	req.index_u16 = address;
	req.data_pu08 = &db;

	PFDL_Execute(&req);

	ps = PFDL_Handler();
	while(ps==PFDL_BUSY)
		ps=PFDL_Handler();
	return db;

}
// *****************************************************************************
// ** Interupts


void EnableIntP0()
{
	PMK0 = 0;
}
void DisableIntP0()
{
	PMK0 = 1;
}
void EnableIntP1()
{
	// TODO:  Clear the correct flag
	PMK0 = 0;
}
void DisableIntP1()
{
	// TODO:  Set the correct flag
	PMK0 = 1;
}
