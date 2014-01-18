#ifndef MICROAPI_DEFS_H
#define MICROAPI_DEFS_H

#include "TypeDefinitions.h"
#include "iodefine.h"

typedef enum
{	/*!This is the Radio interrupt */
	NIRQ = 0
} InterruptTypes;

/*! \details Describes time of day
 */
struct TimeOfDay {
	U8 hour;	/*! Hour */
	U8 minute;	/*! Minute */
	U8 second;	/*! Second */
};

typedef struct
{
	U8 TriggerMode;
	U8 ConversionMode;
	U8 ChannelSelectionMode;
	U8 ConversionRate;
	U8 HardwareTrigger;
	U8 ReferenceSource;
	U8 LimitCheckMode;
	U8 SnoozeMode;
	U8 UpperLimit;
	U8 LowerLimit;
} tAnalogConfiguration;

typedef enum
{
	k9600,
	k19200,
	k38400,
	k57600,
	k76800,
	k115200
} tBaudRates;

// ******************************************************************************************************************************
// *** Public API ***
/*! \details This function initializes the API.  When done, the micro is in its post reset default state.
 * \return Reset reason.  Microcontroller dependent.  Refer to particular implementation.
 */
U8 InitializeMicroAPI(void);

/*! \details This function reads one byte from the SPI from a given location
 * \return Byte that was read.
 */
U8 ReadCharSPI(U8 reg /*! Register (location) to read */) reentrant;

/*! \details Write a single byte to a specific location on the SPI port
 * \return 0 if successful, >=1 if not.
 */
void WriteCharSPI(U8 reg /*! Register */, U8 data /*! Byte to write */) reentrant;

/*! \details This function writes multiple bytes to a specific location on the SPI2 port
 * \return 0 if successful, >=1 if not.
 */
void WriteCharSPIMultiple(
		U8 reg		/*! Register to write */ ,
		U8 count	/*! Number of bytes to write */,
		U8* buffer	/*! Pointer to buffer containing data to write */
	) reentrant;

/*! \details Read multiple bytes from the SPI port
 *
 */
void ReadCharSPIMultiple(
		U8 address	/*! Address to read */,
		U8 count	/*! Number of bytes to read from SPI port */,
		U8 *receiveBuffer	/*! Pointer to buffer to hold received bytes */
	) reentrant;

/*! \details This function reads one byte from the SPI2 from a given location
 * \return Byte that was read.
 */
U8 ReadCharSPI2(U8 reg /*! Register (location) to read */) reentrant;

/*! \details Write a single byte to a specific location on the SPI2 port
 * \return 0 if successful, >=1 if not.
 */
void WriteCharSPI2(U8 reg /*! Register */, U8 data /*! Byte to write */) reentrant;

/*! \details This function writes multiple bytes to a specific location on the SPI2 port
 * \return 0 if successful, >=1 if not.
 */
void WriteCharSPIMultiple2(
		U8 reg		/*! Register to write */,
		U8 count	/*! Number of bytes to write */,
		U8* buffer	/*! Pointer to buffer containing data to write */
	) reentrant;

/*! \details Read multiple bytes from the SPI2 port
 *
 */
void ReadCharSPIMultiple2(
		U8 address			/*! Address to read */,
		U8 count			/*! Number of bytes to read from SPI port */,
		U8 *receiveBuffer	/*! Pointer to buffer to hold received bytes */
	) reentrant;

/*! \details Read a character from the receive buffer for UART0
 *  \return Character from buffer if available, null if not
 */
U8 ReadCharUART0(void);

/*! \details Write a character to the transmit buffer for UART0
 *  \return None
 */
void WriteCharUART0(U8 charToWrite /*! Character to write to buffer */);

/*! \details Get the number of characters in UART0 receive buffer
 *  \return Count
 */
U8 BufferCountUART0(void);

/*! \details Peek a byte out of the buffer without advancing the pointer
 *  \return Byte if a byte is available, 0 if a byte is not.  Note that 0 can be a valid byte in the buffer,so do not use PeekByte to determine if a byte exists.
 */
U8 Uart0PeekByte(void);
/*! \details Peek a series of bytes out of the buffer without advancing the pointer
 *
 */

void Uart0PeekBytes(U8 count	/*! Number of bytes to peek */,
					U8 *buffer	/*! Buffer to put peeked bytes into */);

/*! \details Read a character from the receive buffer for UART1
 *  \return Character from buffer if available, null if not
 */
U8 ReadCharUART1(void);

/*! \details Write a character to the transmit buffer for UART0
 *  \return None
 */
void WriteCharUART1(U8 charToWrite/*! Character to write to buffer */);

/*! \details Get the number of characters in UART1 receive buffer
 *  \return Count
 */
U8 BufferCountUART1(void);

/*! \details Peek a byte out of the buffer without advancing the pointer
 *  \return Byte if a byte is available, 0 if a byte is not.  Note that 0 can be a valid byte in the buffer,so do not use PeekByte to determine if a byte exists.
 */
U8 Uart1PeekByte(void);

/*! \details Peek a series of bytes out of the buffer without advancing the pointer
 *
 */
void Uart1PeekBytes(U8 count/*! Number of bytes to peek */,
					U8 *buffer/*! Buffer to put peeked bytes into */);
/*! \details Set the baud rate of UART1
 *
 */
void SetUART1BaudRate(tBaudRates baudRate /*! Baud rate */);

/*! \details Gets the current UART baud rate
 *  \return Baudrate
 */
U8 GetUART1BaudRate(void);

/*! \details Reads one char(byte) from a specified address on the IIC port
 *  \return Character read
 */
U8 ReadCharIIC(U8 address/*! Address to read from */);

/*! \details Reads specified number of characters from a specified starting address
 *  \return 0 if successful, >=1 if not.l
 */
U8 ReadMultipleIIC(U8 count/*! Number of bytes to read */,
		U8 address/*! Address  to start at*/,
		U8 *buffer/*! Buffer to hold received bytes */);

/*! \details Write a char(byte) to a specified address on the IIC port
 *  \return 0 if successful, >=1 if not.
 */
U8 WriteCharIIC(U8 address/*! Address to write to*/,
		U8 byteToWrite /*! Byte to write to address */);

/*! \details Write specified number of characters to specified starting address
 *  \return 0 if successful, >=1 if not.
 */
U8 WriteMultipleIIC(U8 count /*! Number of bytes to write*/,
		U8 address /*! Starting address */,
		U8 *buffer /*! Buffer to write from */);

/*! \details Turn analog converter on
 *  \return none
 */
void AnanlogConversionOn(void);

/*! \details Turn analog converter off
 *  \return none
 */
void AnalogConversionOff(void);

/*! \details Start converting analog
 *  \return none
 */
void AnalogConversionStart(void);

/*! \details Stop converting analog
 *  \return none
 */
void AnalogConversionStop(void);

/*! \details Configure the analog converter
 *  \return none
 */
void AnalogConverterConfigure(tAnalogConfiguration *config /*! Configuration data */);

/*! \details Get result of last conversion (10 bits)
 *  \return 10 bit result in 16 bit format
 */
U16 AnalogGet10BitResult(void);

/*! \details Get result of last conversion (8 bits)
 *  \return 10 bit result in 8 bit format
 */
U8 AnalogGet8BitResult(void);

/*! \details Set the input channel
 *  \return none
 */
void AnalogSetInputChannel(U8 channel /*! Input channel */);

/*! \details Starts the interval timer.  Handle1MsInterval() will be called every millsecond
 *  \return none
 */
void StartIntervalTimer(void);

/*! \details Stops the interval timer.  Handle1Msnterval() will not be called after this returns.
 * \return none
 */
void StopIntervalTimer(void);

/*! \details Resets the radio by bringing the reset pin high for a period and then low
 *
 */
void ResetRadio(void);

/*! \details Sets up the IO to receive a packet
 *
 */
void SetIOForReceive(void);

/*! \details Sets up the IO to transmit a packet
 *
 */
void SetIOForTransmit(void);

/*! \details Set the main clock to use the sub oscillator
 *  \return none
 */
void SetMainClockToSubOscillator(void);
/*! \details Turns the high speed oscillator off
 *  \return none
 */
void InternalHSOscillatorOff(void);

/*! \details Turns the high speed oscillator on
 *  \return none
 */
void InternalHSOscillatorOn(void);

/*! \details Turn the sub oscillator off
 *  \return none
 */
void SubOscillatorOff(void);

/*! \details Turn the sub oscillator on
 *  \return none
 */
void SubOscillatorOn(void);

/*! \details Set the sub-oscillator mode
 *  \return none
 */
void SetSubOscillatorMode(U8 mode /*! 0=low power, 1=normal, 2=Ultra low power*/);

/*! \details Turn on real time clock
 *  \return none
 */
void RealTimeClockOn(void);

/*! \details Turn off real time clock
 *  \return none
 */
void RealTimeClockOff(void);

/*! \details Enable 1Hz output
 *  \return none
 */
void EnableRTC1HZ(void);

/*! \details Disable 1Hz output
 *  \return none
 */
void DisableRTC1HZ(void);

/*! \details Sets 12 or 24 hour mode
 *  \return none
 */
void SetRTCHourMode(U8 mode /*! 0=12 hour, 1=24 hour*/);

/*! \details Sets the constant period interrupt
 *  \return none
 */
void SetCPI(U8 mode /*! 0=Off, 1= .5sec, 2=1 sec, 3=1 minute, 4=1 hour, 5=1 day, 6=1 month */);

/*! \details Enables RTC alarm
 *  \return none
 */
void EnableRTCAlarm(U8 enableInterrupt /*! 0=disable, 1=enable */);

/*! \details Disables RTC alarm
 *  \return none
 */
void DisableRTCAlarm(void);

/*! \details Hits the watchdog timer
 * \return none
 */
void HitWDT(void);

/*! \details Erase all of the persistent storage area (data flash)
 *
 */
void ErasePersistentArea(void);

/*! \details Write a string of bytes to the persistent storage area (data flash)
 *
 */
void WritePersistentValue(U16 address /*! Starting address */,
							U8 *value/*! Pointer to string of bytes */,
							U8 count /*! Number of bytes to write */);

/*! \details Read a byte from the persistent storage area (data flash)
 *
 */
U8 ReadPersistentValue(U16 address /*! Address to read */);

/*! \details Enable INTP0
 *
 */
void EnableIntP0(void);

/*! \details Disables INTP0
 *
 */
void DisableIntP0(void);
/*! \details Enables INTP1
 *
 */
void EnableIntP1(void);
/*! \details Disables INTP1
 *
 */
void DisableIntP1(void);

#define INITIALIZEDVALUE 0x55

#define DI()	__disable_irq()
#define EI()	__enable_irq()
#define NOP()	__nop()

// Disable interrupts
#define DisableInterrupts	DI()
// Enable interrupts
#define EnableInterrupts	EI()

// Get the interrupt pin status for the radio
#define NIRQStatus IRQ
// Gets the status of the SDN pin
#define SDNPin SDN

#define kAnalogTriggerSoftware			0x00
#define kAnalogTriggerHardwareNoWait	0x80
#define kAnalogTriggerHardware			0xc0

#define kAnalogConversionModeSequential	0x00
#define kAnalogConversionModeOneShot	0x20

#define kAnalogHardwareTriggerINTTM01	0x00
#define kAnalogHardwareTriggerINTRTC	0x02
#define kAnalogHardwareTriggerINTIT		0x03

#define kAnalogChannelSelectMode	0x00
#define kAnalogChannelScanMode		0x40

#define kAnalogPositiveReferenceVDD			0x00
#define kAnalogPositiveReferenceP20			0x40
#define kAnalogPositiveReferenceInternal	0x80
#define kAnalogNegativeReferenceVSS			0x00
#define kAnalogNegativeReferenceP21			0x20

#define kAnalogLimitCheckInband			0x00
#define kAnalogLimitCheckOutsideBand	0x08

#define kAnalogSnoozeEnabled	0x00
#define kAnalogSnoozeDisabled	0x04

#define kAnalogResolution10Bits	0x00
#define kAnalogResolution8Bits	0x01

// This must be defined in the radioapi.  It will be called by the microapi when an interrupt happens that needs to be 
// handled by the radioapi
extern void HandleInterrupt(U8 intType);
// This must be defined in radioapi.  It will be called once every millisecond by the Timer3 interrupt
extern void Handle1MsInterrupt(void);
// This must be defined in radioapi.  It will be called once every second by the RTC interrupt
extern void Handle1SecInterrupt(void);

// ******************************************************************************************************************************
#define KEY_WAITTIME	(8U)	/* Wait 250 */

#define NSSpin			P12_bit.no0
#define NSS2pin			P14_bit.no7
#define pinPowerLock	P2_bit.no3

#define pinDIO0 P7_bit.no0
#define pinDIO1 P7_bit.no1
#define pinDIO2 P7_bit.no2
#define pinDIO3 P7_bit.no3
#define pinDIO4 P13_bit.no7
#define pinDIO5 P5_bit.no0
#define pinDIO6 P1_bit.no6
#define pinDIO7 P1_bit.no7
#define pinDIO8 P1_bit.no0
#define pinDIO9 P14_bit.no7
#define pinDIO10 P2_bit.no6
#define pinDIO11 P1_bit.no3
#define pinDIO12 P1_bit.no4
#define pinDIO13 P1_bit.no5
#define pinDIO14 P12_bit.no1
#define pinDIO15 P6_bit.no2

#define HIGH 1
#define LOW 0

#endif
/*!
 * @}
 */
