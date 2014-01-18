/*************************************************************************************
**																					**
**	radioapi.h			Si1000 RFIC													**
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
// Revision		Date	Reviser		Description
// ===================================================================================
// ===================================================================================

/*! \addtogroup Radio Radio API
 *
 * \ingroup PortedAPIs
 * @{
 * \details This is the radio API for the SX1231H radio.
 *
 */
#ifndef RADIOAPI_DEFS_H
#define RADIOAPI_DEFS_H

// these defines determine which micro controller api is used
//#define MICRO_IS_SI1000
#define MICRO_IS_RL78
// #define MICRO_IS_RX62N

#ifdef MICRO_IS_SI1000
#include "..\..\MicrocontrollerAPI\Si1000\microapi.h"
#include "si1000_defs.h"
#endif

#ifdef MICRO_IS_RL78
#include "..\..\MicrocontrollerAPI\RL78\microapi.h"
#include "sx1231_defs.h"
#endif


enum
{
	kListenTimer,
	MAXTIMERS
};
#define FHSSCHANNELS 50

/*! \details Initialization structure for RadioAPI
 *
 */
typedef struct
{
	UU32 MacAddress; /*! MAC address of radio */
	U8 HopTable; /*! Hop table to use */
	U16 FhssStepSize; /*! Channel spacing between FHSS channels in khz */
	U8 GausianEnabled; /*! Non zero if GFSK is to be used.  Bt will be 0.5 for GFSK and 1.0 for FSK*/
	UU32 NetworkId; /*! Network id  */
} tRadioInitialization;

/*! \details Defines all of the different packet types that are available
 */
typedef enum
{
	/*! Unicast packet (point to point) with acknowledgment */
	kUniAckPacketType=0,
	/*! Unicast packet(point to point) without acknowledgment */
	kUniNoAckPacketType,
	/*! Multicast packet.  This is a broadcast packet to everyone on the network */
	kMulticastPacketType,
	/*! Acknowledgment packet.  This is sent in response to a UNIACK packet	 */
	kAckPacketType,
	/*! Unicast packet (point to point) with acknowledgment  with hopping*/
	kHoppingUniAckPacketType=128,
	/*! Unicast packet(point to point) without acknowledgment with hopping*/
	kHoppingUniNoAckPacketType,
	/*! Multicast packet.  This is a broadcast packet to everyone on the network with hopping*/
	kHoppingMulticastPacketType,
	/*! Acknowledgment packet.  This is sent in response to a UNIACK packet	 with hopping*/
	kHoppingAckPacketType,
} tPacketTypes;

/*! \details Enumerates the different listen modes available
 *
 */
typedef enum
{
	/*! Listen on the current channel until a packet is received or the mode is changed via OpenRFSendPacket or OpenRFSleep */
	kContinuous=1,
	/*! Listen for 48-bit periods on each channel in turn until a packet is received or the mode is changed via OpenRFSendPacket or OpenRFSleep */
	kContinuousScan=129,
	/*! Listen for 48-bit periods on the current channel then sleep for the remainder of the sleep period.  Exits on packet reception or mode change. */
	kPeriodic=2,
	/*! Listen for 48-bit periods on the each channel then sleep for the remainder of the sleep period.  Exits on packet reception or mode change. */
	kPeriodicScan=130
} tListenModes;
/*! \details Defines all of the "over the air" data rates that are available.  Both standard and rounded rates are available
 */

typedef enum
{
	/*! 1200 BAUD */
	k1200BPS,
	/*! 2400 BAUD */
	k2400BPS,
	/*! 4800 BAUD */
	k4800BPS,
	/*! 9600 BAUD */
	k9600BPS,
	/*! 19200 BAUD */
	k19200BPS,
	/*! 38400 BAUD */
	k38400BPS,
	/*! 57600 BAUD */
	k57600BPS,
	/*! 76800 BAUD */
	k76800BPS,
	/*! 153600 BAUD */
	k153600BPS,
	/*! 12.5K BAUD */
	k12_5KBPS,
	/*! 25K BAUD */
	k25KBPS,
	/*! 50K BAUD */
	k50KBPS,
	/*! 100K BAUD */
	k100KBPS,
	/*! 150K BAUD */
	k150KBPS,
	/*! 200K BAUD */
	k200KBPS,
	/*! 250K BAUD */
	k250KBPS,
	/*! 300K BAUD */
	k300KBPS
} tDataRates;

/*! \details Defines all of the different radio modes that are available
 */
typedef enum
{
	/*! All blocks disabled */
	kSleepMode,
	/*! Top regulator and crystal osc enabled */
	kStandbyMode,
	/*! Frequency synthesizer is enabled */
	kFSMode,
	/*! Frequency synthesizer and transmitter are enabled */
	kTransmitMode,
	/* Frequency synthesizer and receiver are enabled */
	kReceiveMode,
	/* Periodically listens for trasnmitter, staying in Standby mode most of the time */
	kListenMode
} tOperatingModes;
enum
{
	kInterruptP0,
	kInterruptP1,
	kInterruptP2,
	kInterruptP3,
	kInterruptP4,
	kInterruptP5,
	kInterruptK0=0x80,
	kInterruptK1,
	kInterruptK2,
	kInterruptK3
};

// ***********************************************************************************
// *** Public API ***

/*! \details Initialize the RadioAPI.  On return, the radio hardware will be configured and will be asleep.
 */
U8 RadioInitialize(tRadioInitialization ini /*! Data structure to initialize radio API */);

/*! \details Send a packet using the radio's built in packet engine.
 *  \return 1=success, 0=error with radio
 */
U8 RadioSendPacket(
		UU32 destAddress	/*! Destination MAC address */ ,
		tPacketTypes packetType	/*! Packet type */,
		U8 length			/*! Length of packet SDU (service data unit or payload) */,
		U8 *SDU				/*! Pointer to buffer containing packet SDU (service data unit or payload) */,
		UU16 preambleCount	/*! Preamble count in bits  */,
		U8 blocking			/*! 1=make this a blocking call, 0=use interrupts instead*/
	);

/*! \details Put radio in receive mode and start listening for an incoming packet
 */
void RadioReceivePacket(
		tListenModes listenMode	/*! Listen mode */,
		U16 period				/*! Listen period in mSec */
	);

/*! \details Change the data rate, Frequency deviation, Rx bandwidth, AFC bandwidth, and low beta afc offset
 *
 */
void RadioSetDataRate(tDataRates dataRate/*!Data rate for TX and RX*/);

/*! \details Set the radio to sleep mode.  This is the lowest power mode of the radio.  In this mode, the radio is completely shut down. .1uA typical in this mode
 *
 */
U8 RadioSleepMode(void);

/*! \details Set the radio to standby mode.  This is the second lowest power mode of the radio.  In this mode, the oscillator is running. 1.25mA typical in this mode
 *
 */
U8 RadioStandbyMode(void);

/*! \details Sets the radio channel.
 */
void RadioSetChannel(U8 channel /*! Desired channel.  Valid channels are 0-24*/);

/*! \details Sets the radio transmit power.  If the RFIC is a 1231H, the PA Boost will automatically be used for the high power setting.
 */
void RadioSetTxPower(U8 power /*! Desired power level.  Bits 6,7,8 turn on PA0,PA1,PA2 respectively.  Bits 0-4 set power level in 1dB increments.  See 3.4.6 in SX1231 datasheet */);

/*! \details Sets the RSSI threshold.  This threshold must be exceeded for a packet reception to happen.
 *
 */
void RadioSetRSSIThreshold(U8 threshold /*! RSSI threshold.  See SX1231 documentation (Section 6.4) for information about this value.*/);

/*! \details Reads RSSI from the SX1231.  This call disables interrupts globally while it executes.
 *  \return RSSI value.   See section 3.4.9 in SX1231 manual for relationship between this value and RSSI.
 */
U8 RadioReadRSSIValue(void);
/*! \details Sets the encryption key
 *
 */
void RadioSetEncryptionKey(U8 *key /*! Pointer to key in memory */,
							U8 length /*! Length of key. MAKE SURE THIS IS NOT LONGER THAN THE KEY ARRAY*/);

/*! \details Sets the sync code (should typically be the network id);
 *
 */
void RadioSetSyncCode(UU32 syncCode /*! Sync code */);

/*! \details Gets the temperature from the radio.
 * \return Temperature value. See 3.4.17 in SX1231 manual for information on this value.
 *
 */
U8 RadioGetTemperature(void);
U8 RadioGetRFICMode(void);

// ******************************************************************************************************
// External event handler declarations

extern void NotifyRadioPacketReceived(tPacketTypes packetType, U8 length, U8 xdata *txBuffer);
extern void NotifyRadioPacketSent(void);
extern void NotifyRadioPacketSendError(void);
extern void NotifyRadioReceiveError(void);
extern void NotifyRadio1Second(void);
extern void NotifyRadio1MilliSecond(void);

// ******************************************************************************************************
// Internal event handlers we are exposing to MicroAPI

// These are the callbacks from the microapi for various interrupt services
extern void HandleInterrupt(U8 intType);
extern void Handle1MsInterrupt(void);
extern void Handle1SecInterrupt(void);

// This is a published callback to the MAC layer that notifies the MAC layer when a packet has
// been received. This is used if ReceivePacket(0) is called.
extern void HandleRxPacket(void);

//*****************************************************************************************************
// Pin definitions

#define pinCRCOK	pinDIO0
#define pinPKTSENT	pinDIO0
#define pinLOCKPLL	pinDIO1
#define pinTIMEOUT	pinDIO1
#define pinFIFONE	pinDIO2
#define pinSYNCADDR	pinDIO3
#define pinTXRDY	pinDIO4
#define pinRSSI		pinDIO4
#define pinMODERDY	pinDIO5

#endif
/*!
 * @}
 */
