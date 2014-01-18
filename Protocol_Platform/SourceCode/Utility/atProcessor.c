/*************************************************************************************
**																					**
**	atProcessor.c		Si1000 processor (8051 derivative							**
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


#include "..\Radio\Si1000\radioapi.h"
#include "..\Includes\openrf_mac.h"

typedef enum 
{
	kDisabled,
	kPlus1,
	kPlus2,
	kPlus3,
	kEnabled
} tAtStates;

tAtStates atState;

U8 xdata atBuffer[8];

void AtInitialize()
{
	atState = kDisabled;
}
void AtProcess()
{
	if(UartBytesAvailable()>0)
	{
		switch(atState)
		{
			case kDisabled:
				if(UartBytesAvailable()<=8)
				{
					UartPeekBytes(UartBytesAvailable(),atBuffer);

				}
				break;
			case kPlus1:
				break;
			case kPlus2:
				break;
			case kPlus3:
				break;
			case kEnabled:
				break;

		}
	}
}