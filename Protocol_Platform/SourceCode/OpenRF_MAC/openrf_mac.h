#ifndef OPENRF_HEADER
#define OPENRF_HEADER

#include "../Radio/SX1231/radioapi.h"

#define kMaxMessageQueueSize 4

/*! \details Enumerates all of the possible states of the OpenRF stack.
 *
 */
typedef enum
{
	kIdle,				/*! Idle state - Lowest radio power state*/
	kTransmitting,		/*! Transmitting state */
	kListening,			/*! Listening for a packet */
	kWaitingForAck,		/*! Waiting for an ack */
	kAckReceived,		/*! Just received an ack*/
	kAckRequired,		/*! Waiting for an ack */
	kReceiving,			/*! Receiving a packet right now */
	kPacketReceived,	/*! A fully qualified packet has been received*/
	kPacketTransmitted,	/*! Last packet is fully transmitted.  If the packet required an ACK, then the ACK has been received */
	kRadioError			/*! An error happened while trying to use the radio */
} tOpenRFStates;

/*! \details Enumerates all possible errors that can happen in the transmit process
 *
 */
typedef enum
{
	kNoAck,				/*! An ack was required but none was received */
	kFifoUnderflow,		/*! The FIFO underflowed, meaning more bytes were extracted than were put in */
	kFifoOverflow,		/*! The FIFO overflowed, meaning too many bytes were put into the FIFO */
	kUndefined			/*! Undefined error */
} tTransmitErrors;

/*! \details Enumerates OpenRF hopping modes
 *
 */
typedef enum
{
	kHopping,			/*! Frequency hopping spread spectrum */
	kDTS				/*! Single frequency operation */
} tHoppingMode;

/*! \details Initializer for OpenRF.  Passes parameters into initialization function
 *
 */
typedef struct
{
	U16 AckTimeout;		/*! Acknowlegement timeout in msec */
	U8 AckRetries;		/*! Number of retries before UniAck packet fails */
	U8 ChannelCount;	/*! Number of channels (usually 25 or 50) */
	U8 GfskModifier;	/*! Set to non-zero if we are to use a gausian filter in modulation chain */
	tDataRates DataRate;/*! Radio data rate */
	UU128 EncryptionKey;/*! Encryption Key - actual bit count is radio dependent */
	UU32 NetworkId;		/*! 32-bit network Id */
	U8 HopTable;		/*! Hop table */
	U8 StartChannel;	/*! Initial channel */
	UU32 MacAddress;	/*! MAC address for this radio */
} tOpenRFInitializer;


extern void NotifyMacPacketReceived(tPacketTypes packetType, UU32 sourceMACAddress, U8 length, U8 *SDU, U8 rssi);
extern void NotifyMacReceiveError(void);
extern void NotifyMac1Second(void);
extern void NotifyMacPacketSent(void);
extern void NotifyMacPacketSendError(tTransmitErrors);
extern void NotifyMac1MilliSecond(void);
/*! 
 * \details Queues a packet for transmission
 * \returns One of results enumeration (OK, MallocFailed, etc)
 */
void OpenRFSendPacket(
	UU32 destAddress		/*! Destination MAC address */,
	tPacketTypes packetType	/*! Type of packet */,
	U8 length				/*! Length of SDU */,
	U8 *txBuffer			/*! Buffer containing SDU */,
	U16 preambleCount		/*! Preamble bit count in bits */
);
/*! \details Initialize the OpenRF stack
 *
 */
void OpenRFInitialize(tOpenRFInitializer ini /*! Initialization structure */);

/*! \details Start listening for a packet
 *
 */
void OpenRFListenForPacket(
	tListenModes mode /*! 0=Continuous, 1=ContinuousScan, 2=Periodic, 3=PeriodicScan */,
	U16 period /*! Sleep period (if applicable) in mSec*/
);

/*!
 * \details This needs to be called once per main loop.  All internal OpenRF processing happens in here.
 * \return Current state of the OpenRF stack.
 */
tOpenRFStates OpenRFLoop(void);

/*! \details Check to see if we can send a packet
 *  \return  0=Not ready to send, >=1 = Ready to send
 */
U8 OpenRFReadyToSend(void);

/*! \details Enter low power sleep mode
 *
 */
void OpenRFSleep(
	U8 level /*! Low power level (0-3 with 3 being lowest power)*/
);

// ***  Macro wrappers for RadioAPI functions ***

#define OpenRFSetHopTable(x)	SetHopTable(x)
#define OpenRFSetTxPower(x)		SetTxPower(x)
#endif
