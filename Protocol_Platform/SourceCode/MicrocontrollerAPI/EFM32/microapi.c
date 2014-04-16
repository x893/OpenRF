#include "microapi.h"
#include "../../Radio/SX1231/radioapi.h"

// Global Variables
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

void InitializeTimers()
{
}

void InitializePorts()
{
}

void InitializeClock()
{
}

void InitializeSPI()
{
	GpioSet(NSSpin);
}

void EnableSPI()
{
}
void DisableSPI()
{
}

void EnableSPI2()
{
}
void DisableSPI2()
{
}
void InitializeIIC()
{
}
void InitializeInterrupts()
{
}

void InitializeUART1(tBaudRates baudRate)
{
}

// ***********************************************************************************
// *** Public API ***

U8 InitializeMicroAPI(void)
{
	InitializePorts();
	InitializeClock();
	InitializeSPI();

	InitializeUART1(k115200);
	InitializeInterrupts();
	GpioSet(NSSpin);

	InitializeTimers();
	RealTimeClockOn();
	StartIntervalTimer();

	return 0;
}

// *****************************************************************************
// ** SPI

U8 ReadCharSPI(U8 reg)
{
	U8 value;
	U8 waitTime = 0xFF;
	// make sure MSB is low for read
	reg &= 0x7F;
	GpioClear(NSSpin);
	// SIO00 = reg;
	while (0)	// !CSIIF00)
	{
		if (--waitTime == 0)
			return 0;
	}

	// SIO00 = 0x55;
	waitTime = 0xFF;
	while (0)	// !CSIIF00)
	{
		if (--waitTime == 0)
			return 0;
	}
	// value = SIO00;
	GpioSet(NSSpin);
	return value;
}

void WriteCharSPI(U8 reg, U8 value)
{
	U16 waitTime = 0xFFFF;

	// make sure MSB is high for write
	reg |= 0x80;
	GpioClear(NSSpin);
	// SIO00 = reg;
	while (0)	// !CSIIF00)
	{
		if (--waitTime == 0)
			return;
	}

	// SIO00 = value;
	waitTime = 0xFFFF;
	while (1)	// !CSIIF00)
	{
		if (--waitTime == 0)
			return;
	}
	GpioSet(NSSpin);
}
void WriteCharSPIMultiple (U8 count, U8 reg,  U8 *addr)
{
	U8 waitTime = 0xFF;
	// make sure MSB is high for write
	reg |= 0x80;
	GpioClear(NSSpin);
	// SIO00 = reg;
	while (0)	// !CSIIF00)
	{
		if (--waitTime == 0)
			return;
	}
	// CSIIF00 = 0U;	/* clear INTCSI00 interrupt flag */
	while(count-- != 0)
	{
		// SIO00 = *(addr++);
		waitTime = 0xff;
		while (0)	// !CSIIF00)
		{
			if (--waitTime == 0)
				return;
		}
		// CSIIF00 = 0U;    /* clear INTCSI00 interrupt flag */
	}
	GpioSet(NSSpin);
}

void ReadCharSPIMultiple(U8 address, U8 count, U8 *receiveBuffer)
{
	U8 waitTime = 0xff;
	// make sure MSB is low for read
	address &= 0x7F;
	GpioClear(NSSpin);
	// SIO00 = address;
	while (0)	// !CSIIF00)
	{
		if (--waitTime == 0)
			return;
	}
    // CSIIF00 = 0U;    /* clear INTCSI00 interrupt flag */
	while (count-- != 0)
	{
		// SIO00 = 0x55;
		waitTime = 0xFF;
		while (0)	// !CSIIF00)
		{
		   if (--waitTime == 0)
			   return;
		}
	    // CSIIF00 = 0U;    /* clear INTCSI00 interrupt flag */
		// *(receiveBuffer++) = SIO20;
	}
	GpioSet(NSSpin);
}

// *****************************************************************************
// ** UART 1

U8 ReadCharUART1(void)
{
	U8 returnValue = 0;
// TODO:
#if 0
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
// TODO:
#if 0
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
#if 0
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
	U8 retVal = 0;
#if 0	// def UART_ENABLED
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
#if 0	// def UART_ENABLED
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
#if 0	// def UART_ENABLED
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
	return microPrivateData.BaudRate;
}

// *****************************************************************************
// ** Analog

void AnanlogConversionOn(void)
{
}
void AnalogConversionOff(void)
{
}
void AnalogConversionStart(void)
{
}
void AnalogConversionStop(void)
{
}
void AnalogConverterConfigure(tAnalogConfiguration *config)
{
}

U16 AnalogGet10BitResult()
{
	return 0;
}
U8 AnalogGet8BitResult()
{
	return 0;
}
void AnalogSetInputChannel(U8 channel)
{
}

// *****************************************************************************
// ** Interval Timer

void StartIntervalTimer()
{
	// Enable the timer and set interval to 1 msec
}

void StopIntervalTimer()
{
	// disable the interrupt
	// disable the timer
}

// *****************************************************************************
// ** Radio IO

void ResetRadio(void)
{
#if 0	// not supported in rfBrick
	U32 resetCounter = 0;
	RadioResetPin = HIGH;
	while(resetCounter<10000)
		resetCounter++;
	RadioResetPin = LOW;
	resetCounter = 0;
	while(resetCounter<1000000)
		resetCounter++;
#endif
}

void SetIOForReceive()
{
	// enable CRCOK/PKTSENT interrupt
}

void SetIOForTransmit()
{

}

// *****************************************************************************
// ** Oscillators

void SetMainClockToSubOscillator(void)
{
}
void InternalHSOscillatorOff(void)
{
}
void InternalHSOscillatorOn(void)
{
}
void SubOscillatorOff(void)
{
}
void SubOscillatorOn(void)
{
}
void SetSubOscillatorMode(U8 mode)
{
}

// *****************************************************************************
// ** Real time clock

void RealTimeClockOn(void)
{
	// enable RTC constant period interrupt
}

void RealTimeClockOff(void)
{
	// disable RTC constant period interrupt
}

void EnableRTC1HZ(void)
{
}
void DisableRTC1HZ(void)
{
}
void SetRTCHourMode(U8 mode)
{
}

void EnableRTCAlarm(U8 enableInterrupt)
{
}
void DisableRTCAlarm(void)
{
}

// *****************************************************************************
// ** WDT

void HitWDT(void)
{
}
// *****************************************************************************
// ** Data FLASH

void ErasePersistentArea()
{
}

void WritePersistentValue(U16 address, U8 *value, U8 count)
{
}

U8 ReadPersistentValue(U16 address)
{
	return 0;

}

// *****************************************************************************
// ** Interupts

void EnableIntP0()
{
}
void DisableIntP0()
{
}
void EnableIntP1()
{
	// TODO:  Clear the correct flag
}
void DisableIntP1()
{
	// TODO:  Set the correct flag
}

UU32 _RTCDateTimeInSecs;
U8 _RTCSeconds = 0;
U8 _RTCHours = 0;		// 24 hours counter
U8 _RTCMinutes = 0;
U8 _RTCDayOfWeek = 0;	// 0=Sunday, 6=Saturday

void RTC_IRQHandler(void)
{
	Handle1SecInterrupt();
	_RTCSeconds++;
	_RTCDateTimeInSecs.U32++;
	if (_RTCSeconds > 59)
	{
		_RTCSeconds=0;
		_RTCMinutes++;
		if (_RTCMinutes > 59)
		{
			_RTCMinutes = 0;
			_RTCHours++;
			if (_RTCHours > 23)
			{
				_RTCHours = 0;
				_RTCDayOfWeek++;
				if (_RTCDayOfWeek > 6)
					_RTCDayOfWeek = 0;
			}
		}
	}
}

void GPIO_ODD_IRQHandler(void)
{
	HandleInterrupt(kInterruptP0);
}

void GPIO_EVEN_IRQHandler(void)
{
	HandleInterrupt(kInterruptP1);
}

void SysTick_Handler(void)
{
	Handle1MsInterrupt();
}
