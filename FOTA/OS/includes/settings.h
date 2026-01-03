#ifndef SETTINGS_H
#define SETTINGS_H

/* SPI Soft Trace */
/*
#define TRACESPI
#define TRACESPI2
*/

/* ****************************************************************
 *  Enable Big Debugger Console Application
 *  0: Disable Debugger
 *  1: Enable Debugger CLI for manual diagnostics
 *
*/
#define MOD_BIGDEBUGGER_ON  1



/* ********************** FILE HEADER *************************************
*  System Definitions and Macros for HW configuration
*  For Freescale ColdFire Architectures
*  Adopted from Ilialex AVR Library
*
*  DESCRIPTION
*  Definitions & Macros related to HW. Rely on PE to reference the relevant functions
*
*  CONTRIBUTOR TAGS
*      AI - Alexopoulos Ilias
*
*  HISTORY:
* 2010 Aug 12:     [AI] Created This File
*
*
****************************************************************************/


/* #define m_LEDALIVE(x)       Bit1_PutVal(x) */
#define m_LEDALIVEOn		LED_MCUStatus_ClrVal()
#define m_LEDALIVEOff		LED_MCUStatus_SetVal()
#define m_LEDALIVEToggle    LED_MCUStatus_NegVal()
/* #define m_LEDALIVEDIROUT    Bit1_SetDir(TRUE); */

#define m_UART0_RxChar(x)   Comm_RecvChar(x)
#define m_UART0_TxChar(x)   Comm_SendChar(x)

#define m_UART0_PutChar(x)	setReg8(UTB0, x)
#define m_UART0_GetChar		getReg8(URB0)
#define m_UART0_GetStatus	getReg8(USR0)

#define m_COMM_RxEna 		CommRxEna_ClrVal()
#define m_COMM_RxDis 		CommRxEna_SetVal()
#define m_COMM_TxEna 		CommTxEna_SetVal()
#define m_COMM_TxDis 		CommTxEna_ClrVal()

/* Software Timers Definitions */
#define c_SWTIM_RXTO	1
#define c_SWTIM_TXTO	2

/* Burster L1 Base Address */
#define c_COMM_L1BASEADDR	'a'

/* BootLoader Configuration Block */
//#define c_ADDR_CFGSTART	0x7F000L		/* Config start address */

/* TWI Addresses for external peripherals */
#define c_TWIADDR_MMA8652			0x1D	/*!< Address of MMA8652 accelerometer */
#define c_TWIADDR_EEPROM			0x50	/*!< EEPROM address (actual range used 0x50-0x57) */
#define c_TWIADDR_TCA9554			0x38	/*!< I2C GPIO */

#ifndef c_UTEST_HOST



#else

#endif

/* #define INLINE __inline__ */
#define INLINE

#endif
