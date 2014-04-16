#include "radioapi.h"

struct
{
	tListenModes 	ListenMode;
	U16 			ListenPeriod;
	U8 				CurrentChannel;
	tOperatingModes	Mode;
	U8				HopIndex;
	U8 				HopTable;
	UU32			MacAddress;
	U8				GfskEnabled;
	U8				ReceiveBuffer[64];
	U16				Timers[MAXTIMERS];
} radioPrivateData;

const U8 _hopTable50[5][50] = {
	{ 0x1a,0x13,0x2e,0x0c,0x16,0x00,0x01,0x19,0x15,0x1b,0x28,0x1c,0x09,0x31,0x0f,0x20,0x1e,0x22,0x03,0x0b,0x2c,0x18,0x17,0x26,0x0a,0x10,0x2a,0x2d,0x07,0x23,0x11,0x1d,0x21,0x30,0x2f,0x27,0x29,0x14,0x06,0x02,0x2b,0x04,0x05,0x24,0x0d,0x0e,0x1f,0x08,0x12,0x25 },
	{ 0x09,0x2c,0x18,0x0b,0x0e,0x2f,0x15,0x29,0x30,0x14,0x1e,0x12,0x08,0x21,0x2e,0x10,0x2d,0x1d,0x0a,0x27,0x28,0x06,0x24,0x07,0x02,0x05,0x1b,0x03,0x26,0x01,0x23,0x00,0x20,0x25,0x31,0x2b,0x2a,0x19,0x1a,0x0d,0x11,0x22,0x0c,0x17,0x04,0x13,0x16,0x1c,0x0f,0x1f },
	{ 0x28,0x09,0x0d,0x0b,0x0c,0x2c,0x1d,0x16,0x06,0x00,0x21,0x2e,0x02,0x08,0x1f,0x22,0x11,0x26,0x1e,0x2a,0x23,0x18,0x27,0x03,0x14,0x25,0x07,0x2b,0x01,0x0a,0x05,0x12,0x1b,0x24,0x1a,0x31,0x17,0x2f,0x19,0x10,0x15,0x04,0x20,0x2d,0x29,0x0f,0x30,0x1c,0x0e,0x13 },
	{ 0x1f,0x15,0x2c,0x0b,0x14,0x07,0x08,0x09,0x0d,0x17,0x21,0x25,0x2b,0x27,0x18,0x04,0x10,0x1b,0x0a,0x0e,0x02,0x1a,0x29,0x2a,0x05,0x2e,0x2f,0x01,0x11,0x16,0x06,0x00,0x22,0x19,0x20,0x1d,0x03,0x2d,0x30,0x12,0x26,0x23,0x31,0x28,0x24,0x1c,0x13,0x0c,0x0f,0x1e },
	{ 0x0f,0x25,0x22,0x15,0x13,0x10,0x20,0x0a,0x2a,0x06,0x1d,0x19,0x2b,0x23,0x18,0x1b,0x14,0x24,0x27,0x2d,0x05,0x09,0x08,0x29,0x02,0x0b,0x30,0x17,0x26,0x12,0x11,0x21,0x00,0x0d,0x1e,0x03,0x2f,0x1f,0x01,0x1c,0x04,0x28,0x07,0x31,0x2e,0x0c,0x16,0x2c,0x0e,0x1a }
};
U8 const _hopTable25[5][25] = {
	{ 0x13,0x03,0x06,0x16,0x0c,0x0a,0x02,0x18,0x01,0x11,0x14,0x0b,0x10,0x09,0x08,0x05,0x0f,0x04,0x00,0x15,0x12,0x0e,0x0d,0x07,0x17 },
	{ 0x0b,0x12,0x16,0x18,0x17,0x0d,0x15,0x01,0x10,0x0f,0x05,0x02,0x07,0x08,0x13,0x0e,0x0a,0x06,0x04,0x03,0x00,0x09,0x14,0x0c,0x11 },
	{ 0x08,0x02,0x0b,0x06,0x0f,0x13,0x0a,0x0d,0x09,0x18,0x07,0x15,0x05,0x0e,0x12,0x00,0x11,0x04,0x14,0x01,0x17,0x10,0x03,0x16,0x0c },
	{ 0x0b,0x0e,0x03,0x15,0x00,0x18,0x17,0x08,0x13,0x0f,0x10,0x0d,0x0c,0x07,0x14,0x0a,0x04,0x11,0x09,0x12,0x05,0x16,0x06,0x01,0x02 },
	{ 0x00,0x16,0x01,0x0e,0x15,0x0b,0x12,0x07,0x06,0x18,0x11,0x13,0x0d,0x14,0x08,0x02,0x05,0x17,0x09,0x0c,0x04,0x0f,0x10,0x0a,0x03 }
};

U8 _registers[0x80];

#define WriteCHARSPI		WriteCharSPI
#define ReadCHARSPI			ReadCharSPI
#define kModeChangeTimeout	8192

// *************************************************************************************************
// Utility functions used internally by RadioAPI
// returns zero if the change times out, one if the mode times out
U8 WaitForModeChange()
{
	U16 timeOut;

	timeOut = kModeChangeTimeout;
	while (timeOut && !(ReadCHARSPI(RegIrqFlags1) & 0x80))
		timeOut--;
	return (timeOut ? 1 : 0);
}

void ReadRegs()
{
	int i;
	for (i = 0x01; i < 0x80; i++)
		_registers[i] = ReadCHARSPI(i);
}

void ClearFIFO()
{
	WriteCHARSPI(RegIrqFlags2, 0x10);
}

void HandleTimeout()
{
	if (radioPrivateData.ListenMode & 0x80)
	{
		radioPrivateData.CurrentChannel++;

		if (radioPrivateData.CurrentChannel > FHSSCHANNELS
		&&	radioPrivateData.ListenMode == kContinuousScan
			)
			radioPrivateData.CurrentChannel = 0;

		// at this point, if we got to the end of the channels and we are in continuous scan mode, the channel will wrap to 0.
		//  If the channel is FHSSCHANNELS, then we are in PeriodicScan mode and we need to stop the process here, put the radio
		// to sleep, and indicate we are sleeping
		if (radioPrivateData.CurrentChannel == FHSSCHANNELS)
		{
			// this ensures we start at the right channel next time
			radioPrivateData.CurrentChannel = 0;
			RadioSetChannel(radioPrivateData.CurrentChannel);
			RadioSleepMode();
		}
		else
		{
			RadioStandbyMode();
			// set the next channel
			RadioSetChannel(radioPrivateData.CurrentChannel);
			// put the radio in receive mode
			WriteCHARSPI(RegOpMode, 0x10);
			WaitForModeChange();
		}
	}
	else
	{
		// if we are in periodic mode, we don't scan.  Once we have the channel's timeout, we go to sleep for the rest of the period
		RadioSleepMode();
	}
}

void HandleReceivedPacket()
{
	U8 *sdu;
	U8 length;
	int i;

	tPacketTypes packetType;
	WriteCHARSPI(RegOpMode, 0x00);
	WaitForModeChange();

	length = ReadCHARSPI(RegFifo);
	if (length > 64)
		NotifyRadioReceiveError();
	else
	{
		packetType = (tPacketTypes)ReadCHARSPI(RegFifo);

		// The remaining bytes go in the buffer
		if (length > 64)
			NotifyRadioReceiveError();
		else
		{
			for (i = 0; i < length; i++)
				radioPrivateData.ReceiveBuffer[i] = ReadCHARSPI(RegFifo);
			sdu = &(radioPrivateData.ReceiveBuffer[0]);
			// Send it all to the next layer up.
			NotifyRadioPacketReceived(packetType, length, sdu);
		}
	}
	// always leave with an empty FIFO
	ClearFIFO();
}

void HopChannel()
{
	//return;
	if (radioPrivateData.HopTable>5)
	{
		radioPrivateData.HopIndex++;
		if (radioPrivateData.HopIndex > 50)
			radioPrivateData.HopIndex = 0;
		RadioSetChannel(_hopTable50[radioPrivateData.HopTable][radioPrivateData.HopIndex]);
	}
	else
	{
		radioPrivateData.HopIndex++;
		if (radioPrivateData.HopIndex > 25)
			radioPrivateData.HopIndex = 0;
		RadioSetChannel(_hopTable25[radioPrivateData.HopTable][radioPrivateData.HopIndex]);
	}
}

// ***********************************************************************************
// *** Interrupt Handlers ***
// These are public functions exposed by radioapi and used by microapi to notify
// when an interrupt happens. These execute in the ISR context.
// Use normal programming precautions.

// called by microcontroller for io based interrupts

void HandleInterrupt(U8 intType)
{
	U8 idata isr1;
	U8 idata isr2;

	// Tx-> PktSent
	// Rx-> CrcOk

	if (intType == kInterruptP0)
	{
		isr1 = ReadCHARSPI(RegIrqFlags1);
		isr2 = ReadCHARSPI(RegIrqFlags2);

		switch (radioPrivateData.Mode)
		{
			case kListenMode:
			case kReceiveMode:

				if (isr2 & 0x04)
				{
					HandleReceivedPacket();
					RadioSleepMode();
				}
				else // if (isr1 & EZRADIOPRO_ICRCERROR)
					NotifyRadioReceiveError();
				break;
			case kTransmitMode:
				// process "packet sent" interrupt
				if (isr2 & 0x08)
				{
					NotifyRadioPacketSent();
					RadioSleepMode();
				}
				else
					NotifyRadioPacketSendError();
				break;
			default:
				break;
		}
	}
	else if (intType == kInterruptP1)
	{
		isr1 = ReadCHARSPI(RegIrqFlags1);
		isr2 = ReadCHARSPI(RegIrqFlags2);

		// TODO: We should never get here in TxMode or in RxMode where the Timeout bit isn't set in ISR1.
		// Therefore, we need to handle those exceptions here
		switch (radioPrivateData.Mode)
		{
			case kListenMode:
			case kReceiveMode:
				if (isr1 & 0x04)
					HandleTimeout();
				break;
			// case kTransmitMode:
			default:
				break;
		}
	}
}

void StateMachine()
{
	switch (radioPrivateData.Mode)
	{
		case kReceiveMode:
			if (radioPrivateData.ListenMode >= kPeriodic)
			{
				// if we are in a periodic listen mode, we need to monitor the listen timer
				if (radioPrivateData.Timers[kListenTimer] > radioPrivateData.ListenPeriod)
				{
					radioPrivateData.Timers[kListenTimer] = 0;
					// we have slept long enough, start the process over again
					WriteCHARSPI(RegOpMode, 0x10);
				}
			}
			break;
		default:
			break;
	}
}

// called by microcontroller every 1mSec.  Timeouts should be tied to this
void Handle1MsInterrupt()
{
	int i;
	for (i = 0; i < MAXTIMERS; i++)
		radioPrivateData.Timers[i]++;
	StateMachine();
	NotifyRadio1MilliSecond();
}

// called by microcontroller every second.  This is used to drive an internal clock
void Handle1SecInterrupt()
{
	NotifyRadio1Second();
}

// ***********************************************************************************
// *** Internal Functions ***
void ClearTimer(U8 timer)
{
	DisableInterrupts;
	radioPrivateData.Timers[timer] = 0;
	EnableInterrupts;
}

// ***********************************************************************************
// *** Public API ***

U8 RadioInitialize(tRadioInitialization ini)
{
	U8 i;
	ResetRadio();
	// force the hop channel to wrap to zero on next hop so we start on zero
	radioPrivateData.HopIndex = 100;
	radioPrivateData.Mode = kSleepMode;
	radioPrivateData.MacAddress.U32 = ini.MacAddress.U32;
	radioPrivateData.HopTable = ini.HopTable;
	radioPrivateData.GfskEnabled = ini.GausianEnabled;

	// Radio starts in sleep mode
	WriteCHARSPI(RegOpMode, 0x00);
	if (ini.GausianEnabled)
	{
		// Packet mode, FSK modulation, Gausian Filter Bt=0.5
		WriteCHARSPI(RegDataModul, 0x02);		// Set AfcLowBetaOn = 1;
		WriteCHARSPI(RegAfcCtrl, 0x20);
	}
	else
	{
		// Packet mode, FSK modulation, no shaping
		WriteCHARSPI(RegDataModul, 0x00);
		// Normal AFC
		WriteCHARSPI(RegAfcCtrl, 0x00);
	}
	// this will hop to hop index 0, the start of the hopping sequence
	HopChannel();

	// Lowest power level, all PA off
	WriteCHARSPI(RegPaLevel, 0);
	// LNA is 50 ohms and manually set to highest gain
	WriteCHARSPI(RegLna, 0x00);
	// AfcAutoClear is off, and Afc automatically runs when module switches to RX mode
	WriteCHARSPI(RegAfcFei, 0x04);

	// Setup DIO pins for receive mode
	// dio0 = PAYLOADRDY, dio1 = TIMEOUT, dio2=FIFONE, dio3=RSSI, dio4=RXRDY, dio5=MODERDY, CLKOUT = off
	WriteCHARSPI(RegDioMapping1, 0x71);
	WriteCHARSPI(RegDioMapping2, 0x87);

	// Set RSSI threshold to -110dBm.  Reception and AFC are triggered from this level.  Nothing happens until RSSI exceeds it.
	WriteCHARSPI(RegRssiThresh, 0xDE);
	// Set the RSSI Threshold timeout to 80 byte times.  This accounts for a packet of 4 preamble, 4 sync, 64 payload, and 2 CRC bytes plus a 4 byte buffer
	// This is needed because the radio will hang if it triggers on a false positive of RSSI threshold and no packet is received.  Since it won't automatically restart the cycle
	// and re-enter the RSSI phase, it never generates a PayloadReady interrupt.  With this timeout value programmed, the radio will let us know when 80 byte times have expired
	// since a valid RSSI with no corresponding packet.  We will check this in our OpenRF loop to catch the problem and reset the receiver portion of the radio.
	WriteCHARSPI(RegRxTimeout2, 0x40);
	// initialize the AES key
	for (i = 0x3E; i <= 0x4D; i++)
		WriteCHARSPI(i, 0x55);

	// set the mode to idle with the sequencer on
	WriteCHARSPI(RegOpMode, 0x00);
	WriteCHARSPI(RegAutoModes, 0x00);
	// Preamble count is 24
	WriteCHARSPI(RegPreambleMsb, 0x00);
	WriteCHARSPI(RegPreambleLsb, 0x18);

	// Config: sync on,FIFO fill on syncaddr interrupt, sync size=8, synctol = 0
	WriteCHARSPI(RegSyncConfig, 0x98);

	// variable length packet, data whitening, crc on, crc autoclear is on, no address filtering
	WriteCHARSPI(RegPacketConfig1, 0xd0);

	// Maximum receive packet length is 48
	WriteCHARSPI(RegPayloadLength, 0x30);

	// interpacketRxDelay = 0, autorxrestarton = off, aes = on
	WriteCHARSPI(RegPacketConfig2, 0x01);

	WriteCHARSPI(RegTestDagc, 0x00);
	ClearFIFO();
	WriteCHARSPI(RegSyncValue1, ini.NetworkId.U8[3]);
	WriteCHARSPI(RegSyncValue1, ini.NetworkId.U8[2]);
	WriteCHARSPI(RegSyncValue1, ini.NetworkId.U8[1]);
	WriteCHARSPI(RegSyncValue1, ini.NetworkId.U8[0]);
	return WaitForModeChange();
}


// Packet types (data length in bits)
//
// UniAck/UniNoAck - [len:8][packettype:8][destaddress:32][srcaddress:32][payload:len*8]
// Multicast - [len:8][packettype:8][srcaddress:32][payload:len*8]
// Ack - [len:8][packettype:8][destaddress:32][srcaddress:32]

U8 RadioSendPacket(UU32 destAddress, tPacketTypes packetType, U8 length, U8 *txBuffer, U16 preambleCount, U8 blocking)
{
	U8 sduLength, i, hopping;
	UU16 uu16;

	// if the MSB of packetType is set, we are supposed to hop
	hopping = packetType & 0x80;
	// only look at the lower 7 bits to get the actual packet type
	packetType &= 0x7F;

	sduLength = length;
	// Setup DIO pins for transmit  mode
	// dio0 = PKTSENT, dio1 = FIFOLVL, dio2=FIFONE, dio3=PLLLOCK, dio4=TXRDY, dio5=MODERDY, CLKOUT = off
	WriteCHARSPI(RegDioMapping1, 0x03);
	WriteCHARSPI(RegDioMapping2, 0x47);

	if (hopping)
		HopChannel();

	// don't touch the FIFO unless we are sure we are in a IDLE mode
	while ((ReadCHARSPI(RegIrqFlags1) & 0x80) == 0)
		ReadRegs();

	if (packetType == kUniAckPacketType || packetType == kUniNoAckPacketType)
	{
		// NOTE: This must be set to TX start on threshold or else the packet send does not work.  That is the purpose of the 0x7F mask
		WriteCHARSPI(RegFifoThresh, ((length + 9) & 0x7F));
		// the first byte must be the length of the packet, but the length count should not include this count.  So we add 5 to account for overhead instead of 6.
		WriteCHARSPI(RegFifo, length + 9);
		// tx start = 0, fifothreshold = length
		// Write the packetType to the first byte in the FIFO
		WriteCHARSPI(RegFifo, packetType);
		// Write the destination MAC
		WriteCHARSPI(RegFifo, destAddress.U8[0]);
		WriteCHARSPI(RegFifo, destAddress.U8[1]);
		WriteCHARSPI(RegFifo, destAddress.U8[2]);
		WriteCHARSPI(RegFifo, destAddress.U8[3]);
		// Write the sender MAC
		WriteCHARSPI(RegFifo, radioPrivateData.MacAddress.U8[0]);
		WriteCHARSPI(RegFifo, radioPrivateData.MacAddress.U8[1]);
		WriteCHARSPI(RegFifo, radioPrivateData.MacAddress.U8[2]);
		WriteCHARSPI(RegFifo, radioPrivateData.MacAddress.U8[3]);

		for (i = 0; i < sduLength; i++)
			WriteCHARSPI(RegFifo, *(txBuffer++));
	}
	else if (packetType == kMulticastPacketType)
	{
		// NOTE: This must be set to TX start on threshold or else the packet send does not work.  That is the purpose of the 0x7F mask
		WriteCHARSPI(RegFifoThresh, ((length + 5) & 0x7F));
		// the first byte must be the length of the packet, but the length count should not include this count.  So we add 5 to account for overhead instead of 6.
		WriteCHARSPI(RegFifo, length+9);
		// tx start = 0, fifothreshold = length
		// Write the packetType to the first byte in the FIFO
		WriteCHARSPI(RegFifo, packetType);
		// Write the sender MAC
		WriteCHARSPI(RegFifo, radioPrivateData.MacAddress.U8[0]);
		WriteCHARSPI(RegFifo, radioPrivateData.MacAddress.U8[1]);
		WriteCHARSPI(RegFifo, radioPrivateData.MacAddress.U8[2]);
		WriteCHARSPI(RegFifo, radioPrivateData.MacAddress.U8[3]);

		for (i=0;i<sduLength;i++)
			WriteCHARSPI(RegFifo,*(txBuffer++));
	}
	else if (packetType==kAckPacketType)
	{
		// the first byte must be the length of the packet, but the length count should not include this count.  So we add 5 to account for overhead instead of 6.
		WriteCHARSPI(RegFifo, 9);
		// tx start = 0, fifothreshold = length
		// Write the packetType to the first byte in the FIFO
		WriteCHARSPI(RegFifo, packetType);
		// Write the destination MAC
		WriteCHARSPI(RegFifo, destAddress.U8[0]);
		WriteCHARSPI(RegFifo, destAddress.U8[1]);
		WriteCHARSPI(RegFifo, destAddress.U8[2]);
		WriteCHARSPI(RegFifo, destAddress.U8[3]);
		// Write the destination MAC
		WriteCHARSPI(RegFifo, radioPrivateData.MacAddress.U8[0]);
		WriteCHARSPI(RegFifo, radioPrivateData.MacAddress.U8[1]);
		WriteCHARSPI(RegFifo, radioPrivateData.MacAddress.U8[2]);
		WriteCHARSPI(RegFifo, radioPrivateData.MacAddress.U8[3]);
	}

	SetIOForTransmit();
	// TODO: shift so high byte == 0
	
	preambleCount >>= 8;
	uu16.U16 = preambleCount;
	WriteCHARSPI(RegPreambleMsb, uu16.U8[1]);
	WriteCHARSPI(RegPreambleLsb, uu16.U8[0]);

   	WriteCHARSPI(RegOpMode, 0x0C);
	radioPrivateData.Mode = kTransmitMode;
	// we can either return here and let the interrupt based event system take over, or if the caller wants us to block until done, we
	// can wait until the packet is completely sent or the radio causes some error that requires exit.
	if (blocking)
	{
		if (!WaitForModeChange())
			return 0;
		// Wait until packet sent flag is true
		while ((ReadCHARSPI(RegIrqFlags2) & 0x08) == 0)
			;
		// go back to sleep mode
	   	WriteCHARSPI(RegOpMode,0x00);
	   	return WaitForModeChange();
	}
	return WaitForModeChange();
}


void RadioReceivePacket(tListenModes listenMode, U16 period)
{
	radioPrivateData.ListenMode = listenMode;
	radioPrivateData.ListenPeriod = period;

	// turn PA off
	WriteCHARSPI(RegPaLevel, 0x00);
	WriteCHARSPI(RegTestPa1, 0x55);
	WriteCHARSPI(RegTestPa2, 0x70);
	WriteCHARSPI(RegOcp, 0x00);

	if (listenMode & 0x80)
	{
		// Setup timeout for scanning
		WriteCHARSPI(RegRxTimeout1, 0x01);
		WriteCHARSPI(RegRxTimeout2, 0);
		radioPrivateData.CurrentChannel = 0;
		RadioSetChannel(0);
	}

	ClearFIFO();
	WriteCHARSPI(RegRssiThresh, 0xA0);
	SetIOForReceive();
	EnableIntP0();
	EnableIntP1();
	radioPrivateData.Mode = kListenMode;

	ClearTimer(kListenTimer);
	// put the radio in receive mode
	WriteCHARSPI(RegOpMode, 0x10);

	// wait until the mode ready indicator shows that the new mode is active and ready to go.
	WaitForModeChange();
}

void RadioSetDataRate(tDataRates dataRate)
{
	U8 brHi, brLo, fdHi, fdLo, bwMan, bwExp, afcMan, afcExp, bo;

	switch(dataRate)
	{
	case k1200BPS:
		brHi = 0x68;
		brLo = 0x28;
		if (radioPrivateData.GfskEnabled)
		{
			//300 hZ fDev
			fdHi = 0x00;
			fdLo = 0x05;
			// 2.6kHz RXBW
			bwMan = 0x02;
			bwExp = 0x07;
			// 50kHz afcBW
			afcMan = 1;
			afcExp = 3;
			// no afc offset required
			bo = 0;
		}
		else
		{
			// 600hZ Fdev
			fdHi=0x00;
			fdLo = 0x09;
			// 2.6kHz RxBW
			bwMan = 0x02;
			bwExp = 0x07;
			// 50kHz afcBW
			afcMan = 1;
			afcExp = 3;
		}
		break;
	case k2400BPS:
		brHi = 0x34;
		brLo = 0x15;
		if (radioPrivateData.GfskEnabled)
		{
			// 600hZ Fdev
			fdHi = 0x00;
			fdLo = 0x09;
			// 2.6kHz RX BW
			bwMan = 0x02;
			bwExp = 0x07;
			// 50kHz afcBW
			afcMan = 1;
			afcExp = 3;
			// no afc offset required
			bo = 0;
		}
		else
		{
			// 1200 Hz Fdev
			fdHi=0x00;
			fdLo = 0x13;
			// 2.6kHz RxBW
			bwMan = 0x02;
			bwExp = 0x07;
			// 50kHz afcBW
			afcMan = 1;
			afcExp = 3;
		}
		break;
	case k4800BPS:
		brHi = 0x1a;
		brLo = 0x0b;
		if (radioPrivateData.GfskEnabled)
		{
			// 2.4kHz Fdev
			fdHi = 0x00;
			fdLo = 0x27;
			// 3.6kHz Rx BW
			bwMan = 0x00;
			bwExp = 0x07;
			// 62.5kHz afcBW
			afcMan = 0;
			afcExp = 3;
		}
		else
		{
			// 2.4kHz Fdev
			fdHi=0x00;
			fdLo = 0x27;
			// 5.2 kHz RxBW
			bwMan = 0x02;
			bwExp = 0x06;
			// 62.5kHz afcBW
			afcMan = 0;
			afcExp = 3;
			// no afc offset required
			bo = 0;
		}
		break;
	case k9600BPS:
		brHi = 0x0d;
		brLo = 0x05;
		if (radioPrivateData.GfskEnabled)
		{
			// 4.8kHz Fdev
			fdHi = 0x00;
			fdLo = 0x48;
			// 7.8 kHz RxBW
			bwMan = 0x00;
			bwExp = 0x06;
			// 62.5kHz afcBW
			afcMan = 0;
			afcExp = 3;
		}
		else
		{
			// 4.8kHz Fdev
			fdHi=0x00;
			fdLo = 0x4e;
			// 10.4kHz RXBW
			bwMan = 0x02;
			bwExp = 0x05;
			// 83.3kHz afcBW
			afcMan = 2;
			afcExp = 2;
			// 488hZ afc offset
			bo = 1;
		}
		break;
	case k19200BPS:
		brHi = 0x06;
		brLo = 0x83;
		if (radioPrivateData.GfskEnabled)
		{
			// 9.6kHz Fdev
			fdHi = 0x00;
			fdLo = 0x98;
			// 15.6 kHz RxBW
			bwMan = 0x00;
			bwExp = 0x05;
			// 83.3kHz afcBW
			afcMan = 2;
			afcExp = 2;
			// 488hZ afc offset
			bo = 1;
		}
		else
		{
			// 9.6kHz Fdev
			fdHi=0x00;
			fdLo = 0x98;
			// 20.8kHz RxBW

			bwMan = 0x02;
			bwExp = 0x04;
			// 100kHz afcBW
			afcMan = 1;
			afcExp = 2;
		}
		break;
	case k38400BPS:
		brHi = 0x03;
		brLo = 0x41;
		if (radioPrivateData.GfskEnabled)
		{
			// 19.2kHz Fdev
			fdHi = 0x01;
			fdLo = 0x47;
			// 31.3kHz RxBW
			bwMan = 0x00;
			bwExp = 0x04;
			// 125kHz afcBW
			afcMan = 0;
			afcExp = 2;
			// 976hZ afc offset
			bo = 15;
		}
		else
		{
			// 19.2kHz Fdev
			fdHi=0x01;
			fdLo = 0x3a;
			// 41.7kHz RxBW
			bwMan = 0x02;
			bwExp = 0x03;
			// 125kHz afcBW
			afcMan = 0;
			afcExp = 2;
		}
		break;
	case k76800BPS:
		brHi = 0x01;
		brLo = 0xa1;
		if (radioPrivateData.GfskEnabled)
		{
			// 38.4kHz Fdev
			fdHi = 0x02;
			fdLo = 0x75;
			// 62.5kHz RxBW
			bwMan = 0x00;
			bwExp = 0x03;
			// 166.7kHz afcBW
			afcMan = 2;
			afcExp = 1;
			// 1952hZ afc offset
			bo = 4;
		}
		else
		{
			// 38.4kHz Fdev
			fdHi=0x02;
			fdLo = 0x75;
			// 83.3kHz RxBW
			bwMan = 0x02;
			bwExp = 0x02;
			// 200kHz afcBW
			afcMan = 1;
			afcExp = 1;
		}
		break;
	case k153600BPS:
		brHi = 0x00;
		brLo = 0x0d;
		if (radioPrivateData.GfskEnabled)
		{
			// 38.4kHz Fdev
			fdHi = 0x02;
			fdLo = 0x75;
			// 125kHz RxBW
			bwMan = 0x00;
			bwExp = 0x02;
			// 333kHz afcBW
			afcMan = 2;
			afcExp = 0;
			// 3904hZ afc offset
			bo = 8;
		}
		else
		{
			// 76.8kHz Fdev
			fdHi=0x04;
			fdLo = 0xeb;
			// 166.7kHz RxBW
			bwMan = 0x00;
			bwExp = 0x02;
			// 400kHz afcBW
			afcMan = 1;
			afcExp = 0;
		}
		break;
	default:
		// default to 9600
		brHi = 0x0d;
		brLo = 0x05;

		if (radioPrivateData.GfskEnabled)
		{
			// 2.4kHz Fdev
			fdHi = 0x00;
			fdLo = 0x27;
			// 7.8 kHz RxBW
			bwMan = 0x00;
			bwExp = 0x06;
			// 62.5kHz afcBW
			afcMan = 0;
			afcExp = 3;
			// 0hZ afc offset
			bo = 0;
		}
		else
		{
			// 4.8kHz Fdev
			fdHi=0x00;
			fdLo = 0x4e;
			// 10.4kHz RXBW
			bwMan = 0x02;
			bwExp = 0x05;
			// 83.3kHz afcBW
			afcMan = 2;
			afcExp = 2;
		}
		break;
	}

	WriteCHARSPI(RegBitrateMsb, brHi);
	WriteCHARSPI(RegBitrateLsb, brLo);
	WriteCHARSPI(RegFdevMsb, fdHi);
	WriteCHARSPI(RegFdevLsb, fdLo);

	WriteCHARSPI(RegRxBw, 0x40 | (bwMan  << 3) | bwExp);
	WriteCHARSPI(RegAfcBw,0x40 | (afcMan << 3) | afcExp);
	
	if (radioPrivateData.GfskEnabled)
		WriteCHARSPI(RegTestAfc, bo);
}

U8 RadioSleepMode(void)
{
	WriteCHARSPI(RegOpMode, 0x00);
	radioPrivateData.Mode = kSleepMode;
	return WaitForModeChange();
}

U8 RadioStandbyMode(void)
{
	WriteCHARSPI(RegOpMode, 0x04);
	radioPrivateData.Mode = kStandbyMode;
	return WaitForModeChange();
}

void RadioSetChannel(U8 channel)
{
	UU32 frf;

	// Fstep = Fxosc / 2^19 => 32Mhz/2^19 = 61
	// Fc = Fstep * RegFrf[23:0]
	// => RegFrf = Fc / Fstep
	// Fc = 902.5MHz + channel * .5Mhz
	// => RegFrf = (902500000 + channel*500000) / 61
	frf.U32 = (14795082 + (U32)channel * (U32)8196) ;

	WriteCHARSPI(RegFrfMsb, frf.U8[2]);
	WriteCHARSPI(RegFrfMid, frf.U8[1]);
	WriteCHARSPI(RegFrfLsb, frf.U8[0]);
}

//
// power		Power
// =====		==========
// 0-15		invalid
// 16			+5 dBm
// 17			+6 dBm
// 18			+7 dBm
// 19			+8 dBm
// 20			+9 dBm
// 21			+10 dBm
// 22			+11 dBm
// 23			+12 dBm
// 24			+13 dBm
// 25			+14 dBm
// 26			+15 dBm
// 27			+16 dBm
// 28			+17 dBm
// 29			+18 dBm
// 30			+19 dBm
// 31			+20 dBm

void RadioSetTxPower(U8 power)
{
	WriteCHARSPI(RegPaLevel, 0x60 + power);
	WriteCHARSPI(RegOcp, 0x0F);
	WriteCHARSPI(RegTestPa1, 0x5D);
	WriteCHARSPI(RegTestPa2, 0x7C);
}

void RadioSetRSSIThreshold(U8 threshold)
{
	WriteCHARSPI(RegRssiThresh, threshold);
}

U8 RadioReadRSSIValue()
{
	U8 om, rssiConfig, rssi;
	DisableInterrupts;
	rssi = 0;
	om = ReadCHARSPI(RegOpMode);
	if (om == 0x10)
	{
		WriteCHARSPI(RegRssiConfig, 0x01);
		rssiConfig = ReadCHARSPI(RegRssiConfig);
		while ((rssiConfig & 0x02)==0)
		{
			rssiConfig = ReadCHARSPI(RegRssiConfig);
			ReadRegs();
			rssi++;
		}
		rssi = ReadCHARSPI(RegRssiValue);
	}
	else
		rssi = 0x01;
	EnableInterrupts;
	return rssi;
}

void RadioSetEncryptionKey(U8 *key, U8 length)
{
	int i;
	for (i = 0; i < length; i++)
		WriteCHARSPI(RegAesKey1 + i, *(key++));
}

void RadioSetSyncCode(UU32 syncCode)
{
	int i;
	for (i = 0; i < 4; i++)
		WriteCHARSPI(RegSyncValue1 + i, syncCode.U8[i]);
}

U8 RadioGetTemperature()
{
	U8 om, temp=0;
	om = ReadCHARSPI(RegOpMode);
	if (om == 0x08 || om == 0x04)
	{
		WriteCHARSPI(RegTemp1, 0x08);
		while ((ReadCHARSPI(RegTemp1) & 0x04))
			;
		temp = ReadCHARSPI(RegTemp2);
	}
	return temp;
}

U8 RadioGetRFICMode()
{
	U8 mode = ReadCHARSPI(RegOpMode);
	return mode;
}
