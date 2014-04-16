#include "openrf_mac.h"
#include <stdlib.h>

enum
{
	kAckTimer,
	kSyncTimer,
	kLockTimer
} timerDefs;

// ***********************************************************************************
// ** Private variables
// ***********************************************************************************
struct
{
	tOpenRFStates macState;
	tPacketTypes txPacketType;
	tPacketTypes rxPacketType;
	U16 timers[4];
	U16 ackTimeout;
	U8 ackRetries;
	U8 ackRetryCounter;
	UU32 rxSourceMAC;
	UU32 rxDestinationMAC;
	UU32 networkId;
	UU32 macAddress;
	U8 operatingMode;
	U8 isLocked;
	U16 frameLength;
	U8 alreadyHopped;
	U8 gfskEnabled;
	U16 listenPeriod;
	tListenModes listenMode;
}  openRFPrivateData;

U8 _rssi;
U16 counterA,x,y;
// Amount of time to queue a packet before transmitting it
#define TXQUEUETIME 7
U8 syncQueued = 0;
U8 syncTransmitted = 0;
extern UU32 _RTCDateTimeInSecs;

// ***********************************************************************************
// ** Event Handlers 
// ***********************************************************************************
void NotifyRadioPacketReceived(tPacketTypes packetType, U8 length, U8 *SDU)
{
	/*
	UU32 sourceMACAddress, destMACAddress;
	UU32 timeStamp;

	if(packetType == kSyncPacketType)
	{
		// if we receive a sync packet, we know it is from our master because it passed the sync word and CRC match. Therefore, we consider ourselves locked now.
		openRFPrivateData.isLocked = 1;
		// reset the timer.  This puts us in sync with the master timewise.  We use this timer to get the time
		// left in the frame.
		openRFPrivateData.timers[kLockTimer] = 0;
		openRFPrivateData.alreadyHopped = 0;
		timeStamp.U8[0]=SDU[0];
		timeStamp.U8[1]=SDU[1];
		timeStamp.U8[2]=SDU[2];
		timeStamp.U8[3]=SDU[3];

		_RTCDateTimeInSecs.U32 = timeStamp.U32;

	}
	else if(packetType>=kMasterUniAckPacketType)
	{
		// The first four bytes of the FIFO should be the MAC address of the dest
		destMACAddress.U8[0] = SDU[0];
		destMACAddress.U8[1] = SDU[1];
		destMACAddress.U8[2] = SDU[2];
		destMACAddress.U8[3] = SDU[3];
		length-=4;
		if(destMACAddress.U32 == openRFPrivateData.macAddress.U32)
			NotifyMacPacketReceived(packetType,sourceMACAddress,length,SDU,_rssi);
	}
	else if(packetType<=kSlaveMulticastPacketType)
	{
		// The first four bytes of the FIFO should be the MAC address of the dest
		destMACAddress.U8[0] = SDU[0];
		destMACAddress.U8[1] = SDU[1];
		destMACAddress.U8[2] = SDU[2];
		destMACAddress.U8[3] = SDU[3];
		// The next four bytes of the FIFO should be the MAC address of the sender
		sourceMACAddress.U8[0] = SDU[4];
		sourceMACAddress.U8[1] = SDU[5];
		sourceMACAddress.U8[2] = SDU[6];
		sourceMACAddress.U8[3] = SDU[7];
		length-=8;
		openRFPrivateData.rxSourceMAC = sourceMACAddress;
		if(destMACAddress.U32 == openRFPrivateData.macAddress.U32)
			NotifyMacPacketReceived(packetType,sourceMACAddress,length,SDU,_rssi);
	}
	else if(packetType==kAckPacketType)
	{
		// The first four bytes of the FIFO should be the MAC address of the dest
		destMACAddress.U8[0] = SDU[0];
		destMACAddress.U8[1] = SDU[1];
		destMACAddress.U8[2] = SDU[2];
		destMACAddress.U8[3] = SDU[3];
		// The next four bytes of the FIFO should be the MAC address of the sender
		sourceMACAddress.U8[0] = SDU[4];
		sourceMACAddress.U8[1] = SDU[5];
		sourceMACAddress.U8[2] = SDU[6];
		sourceMACAddress.U8[3] = SDU[7];
		length-=8;
		openRFPrivateData.rxSourceMAC = sourceMACAddress;
		// if the ACK is for our last transmission, record the transmission as complete.  Otherwise ignore it.
		if(sourceMACAddress.U32 == openRFPrivateData.rxDestinationMAC.U32)
			NotifyMacPacketSent();
	}

	if((packetType == kSlaveUniAckPacketType)||(packetType == kMasterUniAckPacketType))
	{
		openRFPrivateData.macState = kAckRequired;
	}
	openRFPrivateData.rxPacketType = packetType;
	*/
}
extern void NotifyRadioReceiveError()
{
	openRFPrivateData.macState = kIdle;
	NotifyMacReceiveError();
}
extern void NotifyRadioPacketSent()
{
	//LEDTX = EXTINGUISH;
	/*
	if(openRFPrivateData.txPacketType == kSyncPacketType)
	{
		// the frame period starts once the sync packet is sent.  All slaves will start their timer here as well, so they should be in sync
		openRFPrivateData.timers[kSyncTimer] = 0;
		openRFPrivateData.macState = kSyncTransmitted;
	}
	else if((openRFPrivateData.txPacketType == kMasterUniAckPacketType)||(openRFPrivateData.txPacketType == kSlaveUniAckPacketType))
		openRFPrivateData.macState = kPacketTransmitted;
	else
	{	
		NotifyMacPacketSent();
	}
	*/
}
extern void NotifyRadioPacketSendError()
{
	NotifyMacPacketSendError(kUndefined);
}
extern void NotifyRadio1Second()
{
	NotifyMac1Second();
}
extern void NotifyRadio1MilliSecond()
{
	U8 i;
	NotifyMac1MilliSecond();
	for(i=0;i<4;i++)
		openRFPrivateData.timers[i]++;

		
}
// ***********************************************************************************
// ** Internal functions
// ***********************************************************************************
void ClearOpenRFTimer(U8 timerNumber)
{
	DisableInterrupts;
	openRFPrivateData.timers[timerNumber] = 0;
	EnableInterrupts;
}

// ***********************************************************************************
// ** Public API
// ***********************************************************************************
U16 timeLeft;
U16 timeRequired;
U16 lockTime;

void OpenRFSendPacket(UU32 destAddress, tPacketTypes packetType, U8 length, U8 *txBuffer, U16 preambleCount)
{
	// send the packet and do not block until complete.
	RadioSendPacket(destAddress, packetType, length, txBuffer, preambleCount, 0);
}

void OpenRFInitialize(tOpenRFInitializer ini)
{
	tRadioInitialization rini;
	//ResetRadio();
	//X69
	openRFPrivateData.gfskEnabled = ini.GfskModifier;

	rini.HopTable = 0;
	rini.MacAddress = ini.MacAddress;
	rini.NetworkId = ini.NetworkId;
	rini.GausianEnabled = ini.GfskModifier;
	rini.FhssStepSize = 100;

	RadioInitialize(rini);
	//openRFPrivateData.macAddress.U32 = macAddress.U32;
	//SetMACAddress(macAddress);
	RadioSetDataRate(ini.DataRate);
	RadioSetEncryptionKey(&ini.EncryptionKey.U8[0],16);
	openRFPrivateData.networkId = ini.NetworkId;
	openRFPrivateData.macAddress = ini.MacAddress;
	openRFPrivateData.ackTimeout = ini.AckTimeout;
	openRFPrivateData.macState = kIdle;
	openRFPrivateData.ackRetries = ini.AckRetries;
	openRFPrivateData.alreadyHopped = 0;
	EnableIntP0();
}

void OpenRFListenForPacket(tListenModes mode, U16 period)
{

	openRFPrivateData.listenMode = mode;
	openRFPrivateData.listenPeriod = period;

	RadioReceivePacket(mode, period);
}
tOpenRFStates OpenRFLoop()
{
	U8 oState = RadioGetRFICMode();

	// Update our MACState based on the current radio state
	switch(oState)
	{
	case 0:
	case 1:
	case 2:
		openRFPrivateData.macState = kIdle;
		break;
	case 3:
		openRFPrivateData.macState = kTransmitting;
		break;
	case 4:
		openRFPrivateData.macState = kListening;
		break;
	default:
		openRFPrivateData.macState = kRadioError;
		break;

	}
	return openRFPrivateData.macState;
}
U8 OpenRFReadyToSend()
{
	if(openRFPrivateData.macState == kTransmitting)
		return 0;
	return 1;
}
void OpenRFSleep(U8 level)
{

}
