/*
 * BootLoaderApp.h
 *
 *  Created on: Apr 14, 2011
 *      Author: alexopoulos
 */
#ifndef BOOTLOADERAPP_H_
#define BOOTLOADERAPP_H_

#include "StdTypes.h"

typedef enum  {
        eBLMainStateInit,
        eBLMainStateChkMem,
        eBLMainStateWaitCmd,
        eBLMainStateWaitUSB,
        eBLMainStateExecApp,
        eBLMainStateWaitDnld
} type_BLMainState;

typedef enum {
    eCHKJSR = 0,
    eCHKBLK = 1,
    eCHKLow = 2,
    eCHKHi  = 3,
    eCHKOK  = 4
} type_ChkVector;

type_BLMainState f_BootLoaderApp_StateInit(void);
type_BLMainState f_BootLoaderApp_StateChkMem(void);
type_BLMainState f_BootLoaderApp_StateWaitCmd(void);
type_BLMainState f_BootLoaderApp_StateExecApp(void);
type_BLMainState f_BootLoaderApp_StateWaitDnld(void);
type_BLMainState f_BootLoaderApp_StateWaitUSB(void);

void f_Init_BootLoaderApp(void);
void f_BootLoaderApp(void);
type_BLStateEnum f_BootLoaderApp_GetCommState(void);
type_BLStateEnum f_BootLoaderApp_GetState(void);
INT8U f_BootLoaderApp_GetMode(void);
PINT8U f_BootLoaderApp_GetVectorState(PINT8U p_len, PINT32 p_CPLDError, PINT32U p_CPLDCount);
status_t f_BootLoaderApp_ChkCPLD(void);
INT32 f_BootLoaderApp_ProgCPLD(void);
void f_BootLoaderApp_GoApp(void);
void f_BootLoaderApp_AbortExec(void);
INT8U f_BootLoaderApp_GetStateExt(PINT8U p_data);
INT8U f_BooLoaderApp_CheckActity(void);

status_t f_BootLoaderApp_ChkBlockBlank(PINT32U p_addr, INT32U v_bytelen);

status_t f_BootLoaderApp_HelperChkVector(PINT8U p_vector, INT32U v_LowAddr, INT32U v_HighAddr);
void f_BootLoaderApp_WriteStart(PINT8U p_addr, PINT8U p_bin_data, INT8U v_len);
void f_BootLoaderApp_HelperGoApp(void);


type_BLStateEnum f_BootLoaderApp_SanityChk(void);
type_BLStateEnum f_BootLoaderApp_HelperDetermineActiveRegion(status_t v_Region1, status_t v_Region2);
void f_BootLoaderApp_HelperSetRegion1(void);
void f_BootLoaderApp_HelperSetRegion2(void);

status_t f_BootLoaderApp_HelperChkSection1(void);
status_t f_BootLoaderApp_HelperChkSection2(void);

status_t f_BootLoaderApp_HelperChkBlockCfg1(void);
status_t f_BootLoaderApp_HelperChkBlockCfg2(void);
status_t f_BootLoaderApp_HelperChkBlockApp1(void);
status_t f_BootLoaderApp_HelperChkBlockApp2(void);
status_t f_BootLoaderApp_HelperChkVectorApp1(void);
status_t f_BootLoaderApp_HelperChkVectorApp2(void);
status_t f_BootLoaderApp_HelperChkBase(PINT8U p_data, INT32U v_len, INT32U v_ExpectedCRC);
status_t f_BootLoaderApp_ChkSectorBlank(PINT32U p_addr, INT32U v_bytelen);
void f_BootLoaderAppHelper_GetMCUID(void);

INT32U f_BootLoaderApp_GetAppSpaceLen(void);
INT32U f_BootLoaderApp_ValidateAddress(INT32U v_addr);
type_bool f_BootLoaderApp_ValidateAddressRegion(INT32U v_addr, INT8U v_region);
type_bool f_BootLoaderApp_ValidateAddressRegionApp(INT32U v_addr, INT8U v_region);
#endif /* BOOTLOADERAPP_H_ */
