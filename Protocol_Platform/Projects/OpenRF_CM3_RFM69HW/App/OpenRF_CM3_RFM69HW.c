#include "microapi.h"
#include "..\..\..\SourceCode\OpenRF_MAC\openrf_mac.h"
#include "..\Utilities\atProcessor.h"

#define ACK		0x06
#define NACK	0x15

#define kMajorSoftwareVersion	0
#define kMinorSoftwareVersion	1

// *****************************************
// AT Commands

#define kATCommandCount	24
const char * atCommands[kATCommandCount] = { "SL", "NA", "DL", "CN", "RE", "EK", "BD", "NB", "SB", "SS", "TE", "%V", "VR", "WS", "RR", "SP", "TL", "TT", "GS", "TP", "TS", "AR", "AT", "HT" };

// AT Commands
enum
{
	kGetMACAddressCommand,
	kGetSetNetworkAddressCommand,
	kGetSetDestinationMACAddressCommand,
	kExitCommandMode,
	kResetToFactoryCommand,
	kGetSetEncryptionKeyCommand,
	kGetSetDataRateCommand,
	kGetSetParityCommand,
	kGetSetStopBitsCommand,
	kGetRSSICommand,
	kGetTemperatureCommand,
	kGetPowerSupplyCommand,
	kGetFirmwareVersion,
	kWriteSettings,
	kGetSetRadioRate,
	kGetSetPacketType,
	kGetSetTriggerLevel,
	kGetSetTriggerTimeout,
	kGetSenderMAC,
	kGetSetTransmitPower,
	kSetTimeReference,
	kGetSetAckRetriesCommand,
	kGetSetAckTimeoutCommand,
	kGetSetHopTable,
	kNullCommand = 0xff
};

// IO Commands
enum
{
	kReadAnalog,
	kReadDigital,
	kSetDigital,
	kSetDigitalTriggerCmd,
	kSetAnalogTriggerCmd
};

UU32	_macAddress;
UU32	_networkId;
UU32	_destinationAddress;
UU128	_encryptionKey;
U16		_transmitTimer;
U16		_transmitTriggerTimeout;
U16		_transmitTriggerTimer;
U8		_operatingMode;
U8		_transmitTriggerLevel;
U8		_transmitTriggerTimerActive = 0;
tPacketTypes	_packetType = kUniAckPacketType;
U8		_packetReceived = 0;
U8		_receivePacketDataBuffer[63];
U8		_receivePacketCount;
U8		_receivePacketType;
UU32	_receivePacketSenderMAC;

U8		_digitalTriggers[5];
UU16	_analogTriggers[6];

U8		_radioDataRate;
U8		_transmitPower;
U8		_uartBaudRate;
U8		_quiet;
U8		_ackRetries;
U16		_ackTimeout;
U8		_hopTable;
extern UU32 _RTCDateTimeInSecs;

// 0 = KRF-TC2
// 1 = KRF-TCMP2
#define kRadioType	1

void WriteCharToUart(U8 ch)
{
	if (ch < 0x0A)
		WriteCharUART1(ch + '0');
	else
		WriteCharUART1(ch - 10 + 'A');
}

void WriteU8ToUart(U8 val)
{
	WriteCharToUart(val >> 4);
	WriteCharToUart(val & 0x0F);
}

void WriteU32ToUart(UU32 val)
{
	int i;

	for (i = 0; i < 4; i++)
	{
		WriteCharToUart(val.U8[3 - i] >> 4);
		WriteCharToUart(val.U8[3 - i] & 0x0F);
	}
}

U32 parseHexU32(char *str)
{
	U32 value = 0;
	char c;

	while ((c = *str++) != '\0')
	{
		if (c >= '0' && c <= '9')
			value = value << 4 | (c - '0');
		else if (c >= 'A' && c <= 'F')
			value = value << 4 | (c - 'A' + 0xA);
		else if (c >= 'a' && c <= 'f')
			value = value << 4 | (c - 'a' + 0xA);
	}
	return value;
}

U8 ReadU32FromUart(U32 *val)
{
	char str[9];
	U8 i;

	for (i = 0; i < (sizeof(str) - 1) && IsATBufferNotEmpty(); i++)
		if (IsATBufferNotEmpty())
			str[i] = GetATBufferCharacter();
	str[i] = '\0';
	*val = parseHexU32(str);
	return i;
}

U8 ReadU16FromUart(U16 *val)
{
	char str[5];
	U8 i;

	for (i = 0; i < (sizeof(str) - 1) && IsATBufferNotEmpty(); i++)
		if (IsATBufferNotEmpty())
			str[i] = GetATBufferCharacter();
	str[i] = 0;
	*val = (U16)parseHexU32(str);
	return i;
}

U8 ReadU8FromUart(U8 *val)
{
	char str[3];
	U8 i;

	for (i = 0; i < (sizeof(str) - 1) && IsATBufferNotEmpty(); i++)
		if (IsATBufferNotEmpty())
			str[i] = GetATBufferCharacter();

	str[i] = '\0';
	*val = (U8)parseHexU32(&str[0]);
	return i;
}

// Callback handler for AT command management
void ATCommand(U8 commandNumber)
{
	U8		retVal, bo;
	U32		val32;
	tOpenRFInitializer ini;

	switch (commandNumber)
	{
		case kGetMACAddressCommand:
			WriteU32ToUart(_macAddress);
			break;

		case kGetSetNetworkAddressCommand:
			if (!IsATBufferNotEmpty())
				WriteU32ToUart(_networkId);
			else if (ReadU32FromUart(&_networkId.U32))
			{
				ini.AckRetries = _ackRetries;
				ini.AckTimeout = _ackTimeout;
				ini.ChannelCount = 25;
				ini.DataRate = (tDataRates)_radioDataRate;
				ini.EncryptionKey = _encryptionKey;
				ini.GfskModifier = 1;
				ini.HopTable = _hopTable;
				ini.MacAddress = _macAddress;
				ini.NetworkId = _networkId;
				ini.StartChannel = 0;
				OpenRFInitialize(ini);
			}
			break;

		case kGetSetDestinationMACAddressCommand:
			if (!IsATBufferNotEmpty())
				WriteU32ToUart(_destinationAddress);
			else if (ReadU32FromUart(&val32))
				_destinationAddress.U32 = val32;
			break;

		case kExitCommandMode:
			ExitCommandMode();
			break;

		case kResetToFactoryCommand:
			break;

		case kGetSetRadioRate:
			if (!IsATBufferNotEmpty())
				WriteCharUART1(_radioDataRate + '0');
			else if (ReadU8FromUart(&_radioDataRate))
			{
				ini.AckRetries = _ackRetries;
				ini.AckTimeout = _ackTimeout;
				ini.ChannelCount = 25;
				ini.DataRate = (tDataRates)_radioDataRate;
				ini.EncryptionKey = _encryptionKey;
				ini.GfskModifier = 1;
				ini.HopTable = _hopTable;
				ini.MacAddress = _macAddress;
				ini.NetworkId = _networkId;
				ini.StartChannel = 0;
				OpenRFInitialize(ini);
			}
			break;

		case kGetSetEncryptionKeyCommand:
			if (!IsATBufferNotEmpty())
			{
				WriteU32ToUart(_encryptionKey.UU32[0]);
				WriteU32ToUart(_encryptionKey.UU32[1]);
				WriteU32ToUart(_encryptionKey.UU32[2]);
				WriteU32ToUart(_encryptionKey.UU32[3]);
			}
			else
			{
				retVal = ReadU32FromUart(&val32);
				if (retVal)
					_encryptionKey.UU32[0].U32 = val32;
				retVal &= ReadU32FromUart(&val32);
				if (retVal)
					_encryptionKey.UU32[1].U32 = val32;
				retVal &= ReadU32FromUart(&val32);
				if (retVal)
					_encryptionKey.UU32[2].U32 = val32;
				retVal &= ReadU32FromUart(&val32);
				if (retVal)
					_encryptionKey.UU32[3].U32 = val32;
				if (retVal)
				{
					ini.AckRetries = _ackRetries;
					ini.AckTimeout = _ackTimeout;
					ini.ChannelCount = 25;
					ini.DataRate = (tDataRates)_radioDataRate;
					ini.EncryptionKey = _encryptionKey;
					ini.GfskModifier = 1;
					ini.HopTable = _hopTable;
					ini.MacAddress = _macAddress;
					ini.NetworkId = _networkId;
					ini.StartChannel = 0;
					OpenRFInitialize(ini);
				}
			}
			break;

		case kGetSetDataRateCommand:
			if (!IsATBufferNotEmpty())
				WriteCharUART1(GetUART1BaudRate() + '0');
			else if (ReadU8FromUart(&_uartBaudRate))
				SetUART1BaudRate((tBaudRates)_uartBaudRate);
			break;

		case kGetSetParityCommand:
			break;
		case kGetSetStopBitsCommand:
			break;

		case kGetRSSICommand:
			WriteU8ToUart(RadioReadRSSIValue());
			break;

		case kGetTemperatureCommand:
			WriteU8ToUart(RadioGetTemperature());
			break;

		case kGetPowerSupplyCommand:
			break;

		case kGetFirmwareVersion:
			WriteCharUART1(kMajorSoftwareVersion + '0');
			WriteCharUART1(kMinorSoftwareVersion + '0');
			break;

		case kWriteSettings:
			ErasePersistentArea();
			WritePersistentValue(0, _networkId.U8, 4);
			WritePersistentValue(4, &_destinationAddress.U8[0], 4);
			WritePersistentValue(8, &_encryptionKey.U8[0], 16);
			WritePersistentValue(24, &_operatingMode, 1);
			bo = GetUART1BaudRate();
			WritePersistentValue(25, &bo, 1);
			WritePersistentValue(26, &_transmitTriggerLevel, 1);
			WritePersistentValue(27, (U8*)(&_transmitTriggerTimeout), 1);
			// mark the persistent memory as initialized so it will be read on the next reset
			bo = 0x55;
			WritePersistentValue(254, &bo, 1);
			break;

		case kGetSetPacketType:
			if (!IsATBufferNotEmpty())
				WriteCharUART1(_packetType + '0');
			else
				ReadU8FromUart((U8 *)(&_packetType));
			break;

		case kGetSetTriggerLevel:
			if (!IsATBufferNotEmpty())
				WriteU8ToUart(_transmitTriggerLevel);
			else
				ReadU8FromUart(&_transmitTriggerLevel);
			break;

		case kGetSetTriggerTimeout:
			if (!IsATBufferNotEmpty())
				WriteU8ToUart(_transmitTriggerTimeout);
			else
				ReadU16FromUart(&_transmitTriggerTimeout);
			break;

		case kGetSenderMAC:
			WriteU32ToUart(_receivePacketSenderMAC);
			break;

		case kGetSetTransmitPower:
			if (!IsATBufferNotEmpty())
				WriteU8ToUart(_transmitPower);
			else if (ReadU8FromUart(&_transmitPower))
				RadioSetTxPower(_transmitPower);
			break;

		case kSetTimeReference:
			if (!IsATBufferNotEmpty() && ReadU32FromUart(&val32))
				_RTCDateTimeInSecs.U32 = val32;
			break;

		case kGetSetAckRetriesCommand:
			if (!IsATBufferNotEmpty())
				WriteCharToUart(_ackRetries);
			else if (ReadU8FromUart(&_ackRetries))
			{
				ini.AckRetries = _ackRetries;
				ini.AckTimeout = _ackTimeout;
				ini.ChannelCount = 25;
				ini.DataRate = (tDataRates)_radioDataRate;
				ini.EncryptionKey = _encryptionKey;
				ini.GfskModifier = 1;
				ini.HopTable = _hopTable;
				ini.MacAddress = _macAddress;
				ini.NetworkId = _networkId;
				ini.StartChannel = 0;
				OpenRFInitialize(ini);
			}
			break;

		case kGetSetAckTimeoutCommand:
			if (!IsATBufferNotEmpty())
			{
				UU16 uu16;
				uu16.U16 = _ackTimeout;
				WriteCharToUart( uu16.U8[1] );
				WriteCharToUart( uu16.U8[0] );
			}
			else if (ReadU16FromUart(&_ackTimeout))
			{
				ini.AckRetries = _ackRetries;
				ini.AckTimeout = _ackTimeout;
				ini.ChannelCount = 25;
				ini.DataRate = (tDataRates)_radioDataRate;
				ini.EncryptionKey = _encryptionKey;
				ini.GfskModifier = 1;
				ini.HopTable = _hopTable;
				ini.MacAddress = _macAddress;
				ini.NetworkId = _networkId;
				ini.StartChannel = 0;
				OpenRFInitialize(ini);
			}
			break;

		case kGetSetHopTable:
			if (!IsATBufferNotEmpty())
				WriteCharToUart(_hopTable);
			else if (ReadU8FromUart(&_hopTable))
			{
				ini.AckRetries = _ackRetries;
				ini.AckTimeout = _ackTimeout;
				ini.ChannelCount = 25;
				ini.DataRate = (tDataRates)_radioDataRate;
				ini.EncryptionKey = _encryptionKey;
				ini.GfskModifier = 1;
				ini.HopTable = _hopTable;
				ini.MacAddress = _macAddress;
				ini.NetworkId = _networkId;
				ini.StartChannel = 0;
				OpenRFInitialize(ini);
			}
			break;

		case kNullCommand:
			WriteCharUART1('O');
			WriteCharUART1('K');
			break;
	}
}

// Load in presets from NV memory
void LoadPresets()
{
	if (ReadPersistentValue(254) == INITIALIZEDVALUE)
	{
		UU16 uu16;

		_networkId.U8[3] = ReadPersistentValue(0);
		_networkId.U8[2] = ReadPersistentValue(1);
		_networkId.U8[1] = ReadPersistentValue(2);
		_networkId.U8[0] = ReadPersistentValue(3);

		_destinationAddress.U8[3] = ReadPersistentValue(4);
		_destinationAddress.U8[2] = ReadPersistentValue(5);
		_destinationAddress.U8[1] = ReadPersistentValue(6);
		_destinationAddress.U8[0] = ReadPersistentValue(7);

		_encryptionKey.U8[15] = ReadPersistentValue(8);
		_encryptionKey.U8[14] = ReadPersistentValue(9);
		_encryptionKey.U8[13] = ReadPersistentValue(10);
		_encryptionKey.U8[12] = ReadPersistentValue(11);
		_encryptionKey.U8[11] = ReadPersistentValue(12);
		_encryptionKey.U8[10] = ReadPersistentValue(13);
		_encryptionKey.U8[9] = ReadPersistentValue(14);
		_encryptionKey.U8[8] = ReadPersistentValue(15);
		_encryptionKey.U8[7] = ReadPersistentValue(16);
		_encryptionKey.U8[6] = ReadPersistentValue(17);
		_encryptionKey.U8[5] = ReadPersistentValue(18);
		_encryptionKey.U8[4] = ReadPersistentValue(19);
		_encryptionKey.U8[3] = ReadPersistentValue(20);
		_encryptionKey.U8[2] = ReadPersistentValue(21);
		_encryptionKey.U8[1] = ReadPersistentValue(22);
		_encryptionKey.U8[0] = ReadPersistentValue(23);

		_quiet = ReadPersistentValue(24);
		SetUART1BaudRate((tBaudRates)ReadPersistentValue(25));
		_transmitTriggerLevel = ReadPersistentValue(26);
		_transmitTriggerTimeout = ReadPersistentValue(27);
		_ackRetries = ReadPersistentValue(28);

		uu16.U16 = _ackTimeout;
		uu16.U8[1] = ReadPersistentValue(29);
		uu16.U8[0] = ReadPersistentValue(30);
		_ackTimeout = uu16.U16;

		_hopTable = ReadPersistentValue(31);
	}
	else
	{
		_macAddress.U32 = 0x11223344;
		_networkId.U32  = 0xaa555aa5;
		_destinationAddress.U32 = 0x44332211;
		_networkId.U32 = 0x11332244;
		_encryptionKey.UU32[3].U32 = 0x1C1D1E1F;
		_encryptionKey.UU32[2].U32 = 0x1E1F1A1B;
		_encryptionKey.UU32[1].U32 = 0x1A1B1C1D;
		_encryptionKey.UU32[0].U32 = 0x11223344;
		_quiet = 0;

		_transmitTriggerLevel = 4;
		_transmitTriggerTimeout = 200;
		_packetType = kMulticastPacketType;
		_radioDataRate = k38400BPS;
	}
}

// Print a string to the UART
void PrintString(const char *str)
{
	if (_quiet)
		return;
	while (*str != '\0')
		WriteCharUART1(*str++);
}

// Print a string to the UART with CRLF
void PrintLine(const char *str)
{
	if (_quiet) return;
	PrintString(str);
	WriteCharUART1('\n');
	WriteCharUART1('\r');
}

// Send a packet over the radio using UART1 received data
void SendPacketFromUART1Data(void)
{
	U8 i;
	U8 count;
	U8 buff[63];	// maximum size of packet is 63 bytes

	count = BufferCountUART1();

	// never send more than the trigger level number of bytes
	if (count > _transmitTriggerLevel)
		count = _transmitTriggerLevel;

	for (i = 0; i < count; i++)
		buff[i] = ReadCharUART1();

	// wait until we are in a state to send the packet.  We must call OpenRFLoop() continuously to update the internal state machine to ensure that
	// ReadyToSend doesn't get stuck on a fail.
	while (!OpenRFReadyToSend())
		OpenRFLoop();

	// TODO: Set the preamable count
	OpenRFSendPacket(_destinationAddress, _packetType, count, buff, 128);
}

/*****************************************************************************************************************************
 ** 		MAIN FUNCION																									**
 *****************************************************************************************************************************/
int main(void)
{
	UU16 analogSample;
	U8 byteCount, i;
	U8 respBuffer[16];
	U8 digitalSample;

	tOpenRFInitializer ini;

	InitializeMicroAPI();

	ErasePersistentArea();
	LoadPresets();

	ini.NetworkId = _networkId;
	ini.MacAddress = _macAddress;
	ini.EncryptionKey = _encryptionKey;
	ini.DataRate = (tDataRates)k38400;

	OpenRFInitialize(ini);

	_transmitTimer = 0;
	ATInitialize(atCommands, kATCommandCount, ATCommand);
	EnableInterrupts;
	PrintLine("OpenRF 0.1A");

	while (1)
	{
		OpenRFLoop();
		ATProcess();

		// if the network mode pin is high, we are an IO slave, otherwise we are a transparent UART radio
		// The master is a transparent UART radio.
		if (GpioRead(pinNetworkMode))
		{
			// Here, we are operating as an IO slave.  We will process requests from our master and sense/change our IO accordingly
			if (_packetReceived)
			{
				_packetReceived = 0;
				// the first byte is the command
				switch (_receivePacketDataBuffer[0])
				{
				case kReadAnalog:
					byteCount = 1;
					respBuffer[0] = NACK;
					if ((_receivePacketDataBuffer[1] > 4) && (_receivePacketCount >= 2))
					{
						AnalogSetInputChannel(_receivePacketDataBuffer[1]);
						analogSample.U16 = AnalogGet10BitResult();
						respBuffer[0] = ACK;
						respBuffer[1] = analogSample.U8[1];
						respBuffer[2] = analogSample.U8[0];
						byteCount = 3;
					}
					OpenRFSendPacket(_receivePacketSenderMAC, _packetType, byteCount, &respBuffer[0], 128);
					break;
				case kReadDigital:

					respBuffer[0] = ACK;
					if (_receivePacketCount >= 2)
					{
						byteCount = 2;
						switch (_receivePacketDataBuffer[1])
						{
						case 0:
							digitalSample = GpioRead(pinDI0);
							break;
						case 1:
							digitalSample = GpioRead(pinDI1);
							break;
						case 2:
							digitalSample = GpioRead(pinDI2);
							break;
						case 3:
							digitalSample = GpioRead(pinDI3);
							break;
						case 4:
							digitalSample = GpioRead(pinDI4);
							break;
						default:
							byteCount = 1;
							respBuffer[0] = NACK;
							break;
						}
					}
					else
					{
						byteCount = 1;
						respBuffer[0] = NACK;
					}
					respBuffer[1] = digitalSample;
					OpenRFSendPacket(_receivePacketSenderMAC, _packetType, byteCount, &respBuffer[0], 128);
					break;
				case kSetDigital:
					respBuffer[0] = ACK;
					if (_receivePacketCount >= 2)
					{
						byteCount = 2;
						switch (_receivePacketDataBuffer[1])
						{
							case 0:
								GpioWrite(pinDO0, _receivePacketDataBuffer[2]);
								break;
							case 1:
								GpioWrite(pinDO1, _receivePacketDataBuffer[2]);
								break;
							case 2:
								GpioWrite(pinDO2, _receivePacketDataBuffer[2]);
								break;
							case 3:
								GpioWrite(pinDO3, _receivePacketDataBuffer[2]);
								break;
							case 4:
								GpioWrite(pinDO4, _receivePacketDataBuffer[2]);
								break;
							default:
								byteCount = 1;
								respBuffer[0] = NACK;
						}
					}
					else
					{
						byteCount = 1;
						respBuffer[0] = NACK;
					}
					respBuffer[1] = digitalSample;
					OpenRFSendPacket(_receivePacketSenderMAC, _packetType, byteCount, &respBuffer[0], 128);
					break;

				case kSetDigitalTriggerCmd:
					byteCount = 1;
					if (_receivePacketDataBuffer[1] < 5 && _receivePacketCount >= 2)
					{
						respBuffer[1] = NACK;
						_digitalTriggers[_receivePacketDataBuffer[1]] = _receivePacketDataBuffer[2];
						byteCount = 2;
					}
					OpenRFSendPacket(_receivePacketSenderMAC, _packetType, byteCount, &respBuffer[0], 128);
					break;

				case kSetAnalogTriggerCmd:
					byteCount = 1;
					respBuffer[0] = NACK;
					if (_receivePacketDataBuffer[1] < 6 && _receivePacketCount >= 2)
					{
						respBuffer[0] = ACK;
						byteCount = 2;
						// TODO: Check ...U8[1]
						// _analogTriggers[_receivePacketDataBuffer[1]].U8[1] = _receivePacketDataBuffer[2];
						// _analogTriggers[_receivePacketDataBuffer[1]].U8[2] = _receivePacketDataBuffer[3];
						_analogTriggers[_receivePacketDataBuffer[1]].U8[0] = _receivePacketDataBuffer[2];
						_analogTriggers[_receivePacketDataBuffer[1]].U8[1] = _receivePacketDataBuffer[3];
					}
					OpenRFSendPacket(_receivePacketSenderMAC, _packetType, byteCount, &respBuffer[0], 128);
					break;
				default:
					respBuffer[0] = NACK;
					OpenRFSendPacket(_receivePacketSenderMAC, _packetType, 1, &respBuffer[0], 128);
					break;
				}
			}
		}
		else
		{
			if (_packetReceived)
			{
				// send whatever we have in the receive buffer to the UART
				_packetReceived = 0;
				for (i = 0; i < _receivePacketCount; i++)
					WriteCharUART1(_receivePacketDataBuffer[i]);
			}
			// Here, if we are not in AT command mode, we need to take whatever data we receive from the UART and forward it.  The
			// data will be forwarded to the module selected by the destination ID.
			if (ATGetState() != kEnabled)
			{
				byteCount = BufferCountUART1();
				if (byteCount > _transmitTriggerLevel)
					SendPacketFromUART1Data();
				else
				{
					// look at the buffer and see what we should do
					if (byteCount > 0 && !_transmitTriggerTimerActive)
					{
						_transmitTriggerTimerActive = 1;
						_transmitTriggerTimer = 0;
					}
					if (_transmitTriggerTimerActive && _transmitTriggerTimer > _transmitTriggerTimeout)
					{
						SendPacketFromUART1Data();
						// de-activate the timer
						_transmitTriggerTimerActive = 0;
					}
				}
			}
		}
	}
}

/*****************************************************************************************************************************
 ** 		EVENT HANDLERS																									**
 *****************************************************************************************************************************/
extern void NotifyMacPacketReceived(
	tPacketTypes packetType,
	UU32 sourceMACAddress,
	U8 length,
	U8 xdata *SDU,
	U8 rssi
	)
{
	int i;
	for (i = 0; i < length; i++)
		_receivePacketDataBuffer[i] = *(SDU++);
	_receivePacketSenderMAC.U32 = sourceMACAddress.U32;
	_receivePacketCount = length;
	_receivePacketType = packetType;
	_packetReceived = 1;
}

void NotifyMacReceiveError()
{
}

void NotifyMac1Second()
{
}

void NotifyMacPacketSent()
{
}

void NotifyMacPacketSendError(tTransmitErrors error)
{
}

void NotifyMac1MilliSecond()
{
	_transmitTimer++;
	_transmitTriggerTimer++;
}
