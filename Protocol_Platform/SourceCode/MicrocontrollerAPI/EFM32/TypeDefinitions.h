#ifndef TYPEDEF_H
#define TYPEDEF_H

/* Status list definition */
#define MD_STATUSBASE		(0x00U)
#define MD_OK				(MD_STATUSBASE + 0x00U) /* register setting OK */
#define MD_SPT				(MD_STATUSBASE + 0x01U) /* IIC stop */
#define MD_NACK				(MD_STATUSBASE + 0x02U) /* IIC no ACK */
#define MD_BUSY1			(MD_STATUSBASE + 0x03U) /* busy 1 */
#define MD_BUSY2			(MD_STATUSBASE + 0x04U) /* busy 2 */

/* Error list definition */
#define MD_ERRORBASE		(0x80U)
#define MD_ERROR			(MD_ERRORBASE + 0x00U)	/* error */
#define MD_ARGERROR			(MD_ERRORBASE + 0x01U)	/* error agrument input error */
#define MD_ERROR1			(MD_ERRORBASE + 0x02U)	/* error 1 */
#define MD_ERROR2			(MD_ERRORBASE + 0x03U)	/* error 2 */
#define MD_ERROR3			(MD_ERRORBASE + 0x04U)	/* error 3 */
#define MD_ERROR4			(MD_ERRORBASE + 0x05U)	/* error 4 */

typedef unsigned char	U8;
typedef unsigned int	U16;
typedef unsigned long	U32;

typedef signed char		S8;
typedef signed int		S16;
typedef signed long		S32;

typedef union
{
	U16 U16;
	S16 S16;
	U8 U8[2];
	S8 S8[2];
} UU16;

typedef union
{
	U32 U32;
	S32 S32;
	UU16 UU16[2];
	U16 U16[2];
	S16 S16[2];
	U8 U8[4];
	S8 S8[4];
} UU32;

typedef union UU64
{
	UU32 UU32[2];
	UU16 UU16[4];
	U16 U16[4];
	U8 U8[8];
} UU64;

typedef union UU128
{
	UU64 UU64[2];
	UU32 UU32[4];
	UU16 UU16[8];
	U16 U16[8];
	U8 U8[16];
} UU128;

#define NULL 0
#define bit unsigned char

// these defines are used to nullify custom keywords used with other compilers
#define xdata
#define idata
#define code
#define reentrant

#endif
