
#ifndef STDTYPES_H
#define STDTYPES_H
/* ********************** FILE HEADER *************************************
*  System Types Definition
*  For Freescale ColdFire Architectures
*  Adopted from Ilialex AVR Library & TechEn PIC
*
*  DESCRIPTION
*  Standard data type definitions
*
*  CONTRIBUTOR TAGS
*      AI - Alexopoulos Ilias
*
*  HISTORY:
* 2010 Aug 12:     [AI] Created This File
*
*
*/

/* *************************************************************************
*  BASIC TYPE DEFINTIONS
************************************************************************* */
typedef signed   char     	INT8;
typedef unsigned char     	INT8U;
typedef signed short        INT16;
typedef unsigned short      INT16U;
typedef signed long int   	INT32;
typedef unsigned long int 	INT32U;

typedef signed   char *    	PINT8;
typedef unsigned char *    	PINT8U;
typedef signed short  *     PINT16;
typedef unsigned short *    PINT16U;
typedef signed long int *  	PINT32;
typedef unsigned long int * PINT32U;

/* Max Slot Trace Record in KMA36 */
#define c_MAXCAPEXIT	180

#define ARCH_BIG_ENDIAN


/* *************************************************************************
*  BASIC TYPE DEFINTIONS for BC5 Application
*/
//typedef signed   char     sint8_t;
//typedef unsigned char     uint8_t;
//typedef signed int        sint16_t;
//typedef unsigned int      uint16_t;
//typedef signed long int   sint32_t;
//typedef unsigned long int uint32_t;

/* *************************************************************************
*  SPECIAL TYPE "BOOL"
*/
typedef enum {bFALSE=0, bTRUE=~bFALSE} type_bool;

/* *************************************************************************
* SPECIAL TYPE "STATUS"
*/
typedef enum {eSUCCESS=0, eFAIL=1, eCHECK=-1} status_t;

#define ARCH_LITTLE_ENDIAN

/////////////////////////////////////////////////////////////////////////////
// Enumeration type reflecting the EagleEye laser sensor API statemachine.
/////////////////////////////////////////////////////////////////////////////
typedef enum  {
		eBLStateEmpty       = 'E',	// Flash Area empty
		eBLStateIntact		= 'A',	// Flash Ok
		eBLStateIncoherent	= 'X',	// Flash Incoherent
		eBLStateNotReady	= 'R',	// Checks have not performed yet
		eBLStateUnknown		= 'U', 	// after a write operation, state is unknown
		eBLStateBusy		= 'B',  // Never return this, busy in operation
		eBLStateVector		= 'V',	// Vector Error (vector out of legal space)
		eBLStateStartEr		= 'C',	// Clean, (Erase) before write not used.
		eBLStateStartWr		= 'W',	// begin Write block
		eBLStateError		= 'O'	// CFM Error
} type_BLStateEnum;



#ifndef NULL
	#define  NULL   0
#endif

#ifndef ARCH_LITTLE_ENDIAN
/* this is Big Endian */
// MSB/LSB 16 & 32 bits
#define MSB16(x) 		(((x) >> 8) & 0x00FFU)
#define LSB16(x)		((x) & 0x00FFU)
#define MSB32(x) 		(((x) >> 16) & 0x0000FFFFU)
#define LSB32(x)		((x) & 0xFFFFU)

#define MSB32_3(x)		( (INT8U) ( ((x) >> 24) & 0xFF) )
#define MSB32_2(x)		( (INT8U) ( ((x) >> 16) & 0xFF) )
#define MSB32_1(x)		( (INT8U) ( ((x) >>  8) & 0xFF) )
#define MSB32_0(x)		( (INT8U) ( ((x) >>  0) & 0xFF) )

#else
/* this is Little Endian */
// MSB/LSB 16 & 32 bits
#define MSB16(x) 		(((x) >> 8) & 0x0000FFFFU)
#define LSB16(x)		((x) & 0x00FFU)
#define MSB32(x) 		(((x) >> 16) & 0x00FFU)
#define LSB32(x)		((x) & 0xFFFFU)

#define MSB32_3(x)		( (INT8U) ( ((x) >>  24) & 0xFF) )
#define MSB32_2(x)		( (INT8U) ( ((x) >>  16) & 0xFF) )
#define MSB32_1(x)		( (INT8U) ( ((x) >>   8) & 0xFF) )
#define MSB32_0(x)		( (INT8U) ( ((x) >>   0) & 0xFF) )

#endif



#ifdef MOD2FIFO
#define MODINCR(addr,modulo) ( (addr+1) & (modulo-1) )
#define MODDECR(addr,modulo) ( (addr-1) & (modulo-1) )
#define MODMASK(addr,modulo) ( (addr  ) & (modulo-1) )

#else
#define MODINCR(addr,modulo) ( (addr == modulo) ? 1 		 : addr+1)
#define MODDECR(addr,modulo) ( (addr == 0) 		  ? modulo-1 : addr-1)
#define MODMASK(addr,modulo) ( (addr > (modulo-1)) ? 0 		 : addr)
#endif


/* *************************************************************************
*  DEFINITION OF 'GLOBAL'
*  may be used in '.h' files to declare global variables
*/

#ifdef MAIN_C
#define GLOBAL /* Global Variable */
#else
#define GLOBAL extern
#endif

#define PNULL (void *)0


#if CF /* Coldfire FUTURE */
    #define c_ROM   __declspec(romdata) const
    #define c_INLINE
    #define c_ROMTYPE __declspec(romdata) const
    #define c_ROMFAR const
#else /* c_UTEST_HOST */
    #define c_ROM
    #define c_INLINE	inline
    #define c_ROMTYPE const
    #define c_ROMFAR
#endif

/* Declares a two byte 'structure', containing 'LSB', and 'MSB' - does not actually create it though */
/* change sequence for Little vs Big endian */
#ifdef ARCH_LITTLE_ENDIAN
struct wordparts {
    INT8U LSB;
    INT8U MSB;
};
#else
struct wordparts {
    INT8U MSB;
    INT8U LSB;
};
#endif

typedef union {
    INT16U crc;
    struct wordparts bytes;
} type_crc16;

typedef enum { eDALLASIDCAB, eDALLASIDL2C } type_DallasIDSelect;


typedef struct {
	INT32U v_HALVersion;
	INT32U v_MCUDevID;
	INT32U v_MCURevID;
	INT32U buf_UUID[3];
} type_MCUID;



#endif /* #ifndef STDTYPES_H */
/* *************** END OF FILE    STDTYPES_H ****************************** */
