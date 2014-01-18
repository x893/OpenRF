/*************************************************************************************
**																					**
**	atprocessor.h		Si1000 processor (8051 derivative							**
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
#ifndef AT_H
#define AT_H

typedef enum
{
	kDisabled,
	kPlus1,
	kPlus2,
	kPlus3,
	kEnabled
} tAtStates;
typedef void (*tCallback)(U8 callbackNumber);
void ATProcess();
void ATInitialize(U8 *commands[], U8 commandCount, tCallback callback);
U8 IsATBufferNotEmpty();
U8 GetATBufferCharacter();
tAtStates ATGetState();
#endif
