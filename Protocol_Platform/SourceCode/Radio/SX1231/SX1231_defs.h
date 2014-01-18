//-----------------------------------------------------------------------------
// Si1000_defs.h
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Register/bit definitions for the Si100x family.
//
//
// Target:         Si100x
// Tool chain:     Keil, SDCC
// Command Line:   None
//
//
// Release 1.0 
//    -EZRadioPRO definitions created by Ken Berrenger.
//    -26 JAN 2010  (FB)
//

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef SX1231_DEFS_H
#define SX1231_DEFS_H

#define RegFifo 		0x00
#define RegOpMode 		0x01
#define RegDataModul 	0x02
#define RegBitrateMsb	0x03
#define RegBitrateLsb   0x04
#define RegFdevMsb		0x05
#define RegFdevLsb		0x06
#define RegFrfMsb		0x07
#define RegFrfMid		0x08
#define RegFrfLsb		0x09
#define RegOsc1			0x0a
#define RegAfcCtrl		0x0b
#define RegListen1		0x0d
#define RegListen2		0x0e
#define RegListen3		0x0f
#define RegVersion		0z10
#define RegPaLevel		0x11
#define RegPaRamp		0x12
#define RegOcp			0x13
#define RegLna			0x18
#define RegRxBw			0x19
#define RegAfcBw		0x1a
#define RegOokPeak		0x1b
#define RegOokAvg		0x1c
#define RegAfcFei		0x1e
#define RegAfcMsb		0x1f
#define RegAfcLsb		0x20
#define RegFeiMsb		0x21
#define RegFeiLsb		0x22
#define RegRssiConfig	0x23
#define RegRssiValue	0x24
#define RegDioMapping1	0x25
#define RegDioMapping2	0x26
#define RegIrqFlags1	0x27
#define RegIrqFlags2	0x28
#define RegRssiThresh	0x29
#define RegRxTimeout1	0x2a
#define RegRxTimeout2	0x2b
#define RegPreambleMsb	0x2c
#define RegPreambleLsb	0x2d
#define RegSyncConfig	0x2e
#define RegSyncValue1	0x2f
#define RegSyncValue2	0x30
#define RegSyncValue3	0x31
#define RegSyncValue4	0x32
#define RegSyncValue5	0x33
#define RegSyncValue6	0x34
#define RegSyncValue7	0x35
#define RegSyncValue8	0x36
#define RegPacketConfig1	0x37
#define RegPayloadLength	0x38
#define RegNodeAdrs		0x39
#define RegBroadcaseAdrs	0x3a
#define RegAutoModes	0x3b
#define RegFifoThresh	0x3c
#define RegPacketConfig2	0x3d
#define RegAesKey1		0x3e
#define RegAesKey2		0x3f
#define RegAesKey3		0x40
#define RegAesKey4		0x41
#define RegAesKey5		0x42
#define RegAesKey6		0x43
#define RegAesKey7		0x44
#define RegAesKey8		0x45
#define RegAesKey9		0x46
#define RegAesKey10		0x47
#define RegAesKey11		0x48
#define RegAesKey12		0x49
#define RegAesKey13		0x4a
#define RegAesKey14		0x4b
#define RegAesKey15		0x4c
#define RegAesKey16		0x4d
#define RegTemp1 		0x4e
#define RegTemp2		0x4f
#define RegTestLna		0x58
#define RegTestPa1		0x5a
#define RegTestPa2		0x5c
#define RegTestDagc		0x6f
#define RegTestAfc		0x71

#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
