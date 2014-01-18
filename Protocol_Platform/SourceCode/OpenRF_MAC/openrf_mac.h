#ifndef OPENRF_HEADER
#define OPENRF_HEADER

#ifdef RADIO_SI1000
	#include "..\Radio\Si1000\radioapi.h"
#endif

#ifdef RADIO_SX1231
	#include "..\Radio\SX1231\radioapi.h"
#endif

#define kMaxMessageQueueSize 4

/*! \details Enumerates all of the possible states of the OpenRF stack.
 *
 */
typedef enum
{
	/*! Idle state - Lowest radio power state*/
	kIdle,
	/*! Transmitting state */
	kTransmitting,
	/*! Listening for a packet */
	kListening,
	/*! Waiting for an ack */
	kWaitingForAck,
	/*! Just received an ack*/
	kAckReceived,
	/*! Waiting for an ack */
	kAckRequired,
	/*! Receiving a packet right now */
	kReceiving,
	/*! A fully qualified packet has been received*/
	kPacketReceived,
	/*! Last packet is fully transmitted.  If the packet required an ACK, then the ACK has been received */
	kPacketTransmitted,
	/*! An error happened while trying to use the radio */
	kRadioError
} tOpenRFStates;

/*! \details Enumerates all possible errors that can happen in the transmit process
 *
 */
typedef enum
{
	/*! An ack was required but none was received */
	kNoAck,
	/*! The FIFO underflowed, meaning more bytes were extracted than were put in */
	kFifoUnderflow,
	/*! The FIFO overflowed, meaning too many bytes were put into the FIFO */
	kFifoOverflow,
	/*! Undefined error */
	kUndefined
} tTransmitErrors;
/*! \details Enumerates OpenRF hopping modes
 *
 */
typedef enum
{
	/*! Frequency hopping spread spectrum */
	kHopping,
	/*! Single frequency operation */
	kDTS
} tHoppingMode;

/*! \details Initializer for OpenRF.  Passes parameters into initialization function
 *
 */
typedef struct
{
	/*! Acknowlegement timeout in msec */
	U16 AckTimeout;
	/*! Number of retries before UniAck packet fails */
	U8 AckRetries;
	/*! Number of channels (usually 25 or 50) */
	U8 ChannelCount;
	/*! Set to non-zero if we are to use a gausian filter in modulation chain */
	U8 GfskModifier;
	/*! Radio data rate */
	tDataRates DataRate;
	/*! Encryption Key - actual bit count is radio dependent */
	UU128 EncryptionKey;
	/*! 32-bit network Id */
	UU32 NetworkId;
	/*! Hop table */
	U8 HopTable;
	/*! Initial channel */
	U8 StartChannel;
	/*! MAC address for this radio */
	UU32 MacAddress;

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
void OpenRFSendPacket(	UU32 destAddress		/*! Destination MAC address */,
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
void OpenRFListenForPacket(tListenModes mode /*! 0=Continuous, 1=ContinuousScan, 2=Periodic, 3=PeriodicScan */,
						U16 period /*! Sleep period (if applicable) in mSec*/);
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
void OpenRFSleep(U8 level /*! Low power level (0-3 with 3 being lowest power)*/);

// ***  Macro wrappers for RadioAPI functions ***

#define OpenRFSetHopTable(x) SetHopTable(x)
#define OpenRFSetTxPower(x) SetTxPower(x)
#endif
/*!
 * @}
 */
