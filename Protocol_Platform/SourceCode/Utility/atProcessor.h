#ifndef AT_H
#define AT_H

#include "TypeDefinitions.h"

typedef enum
{
	kDisabled,
	kPlus1,
	kPlus2,
	kEnabled
} tAtStates;

typedef void (*tCallback)(U8 callbackNumber);
void ATProcess(void);
void ATInitialize(U8 *commands[], U8 commandCount, tCallback callback);
U8 IsATBufferNotEmpty(void);
U8 GetATBufferCharacter(void);
tAtStates ATGetState(void);

#endif
