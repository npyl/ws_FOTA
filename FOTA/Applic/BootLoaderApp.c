/* ********************** BootLoaderApp.c ***********************************
    Title:
    Author:   Alexopoulos Ilias
    Date:     Apr 14, 2011
    Purpose:
    Configuration: CodeWarrior MCF & HW
    Last updated:   Apr 14, 2011

    Version     Date            Description
    V0.00       Apr 14, 2011        Initial Release

************************************************************************** */
/*
 * BootLoaderApp.c
 *
 *  Created on: Apr 14, 2011
 *      Author: alexopoulos
 */

#include "Flash.h"
#include "StdTypes.h"

#include "BootLoaderApp.h"

#include "utils.h"


#define c_DATARECORDLEN         64

#define c_TIMEWAIT_ms           8000UL         // Wait 8sec before jump to APP.
#define c_USBWAIT_ms            2000UL         // Wait 2sec before jump to APP.




// Sector Erase: 2K.
// Minimum Write: 4 bytes.
/* v_blstate
        eBLStateEmpty       = 'E',
        eBLStateIntact      = 'A',
        eBLStateIncoherent  = 'X',
        eBLStateNotReady    = 'R',
        eBLStateBusy        = 'B',
        eBLStateCPLD        = 'C' // CPLD differs, Application Intact
        eBLStateVector      = 'V' // Application Intact, Reset Vector Invalid
        eBLStateError       = 'O'   // CFM Error
 */

/*
Part Number Flash Size (in KBytes) RAM Size (in KBytes) Package Chip Identification Number

*/

#define c_CPUID_Count       2

typedef struct {
    INT32U v_cpuid;
    INT16U v_flash;
    INT16U v_ram;
    INT8U buf_name[20];
} type_cpuid;


typedef union
{
    struct
    {
        char v_cfgcrc1;
        char v_cfgcrc2;
        char v_appcrc1;
        char v_appcrc2;
        char v_appvec1;
        char v_appvec2;
        char v_active_region;
    } st_vector;
    INT8U buf_vecstate[20];         // length should be larger than st_vector struct
} type_vectorstate;


typedef struct
{
    PINT8U v_CurrAddr;                  /*!< Current Address for writing data   */
    INT32U v_CurrSector;                /*!< Current sector we are writing data */
    INT32U v_CurrLen;                   /*!< Length of writing block            */
    INT32U v_LastSectorErased;          /*!< 2K Size                            */
    type_BLStateEnum v_blstate;         /*!< Bootloader status byte of memory state     */
    type_BLMainState v_MainState;       /*!< FSM State of BootLoader            */
    type_ChkVector v_ChkVectorStatus;   /*!< Vector test summary status         */
    type_vectorstate u_vecstate;        /*!< Bootloader status byte */
    INT32U v_AppStart;                  /*!< Configurable start address of active application (was hard coded) */
    INT32U v_AppStop;                   /*!< Configurable end address of active application (was hard coded) */
    INT32U v_CfgStart;                  /*!< Configurable start address of active config block (was hard coded) */
    INT32U v_CfgStop;                   /*!< Configurable end address of active config block (was hard coded) */
    PINT32U p_Act_appbase;              /*!< Active Application Base            */
    INT32U v_Act_appstack;              /*!< Active Application Stack           */
    INT32U v_Act_vectoroffset;          /*!< Active Vector Offset               */
    INT8U buf_datarecord[c_DATARECORDLEN];  /*!< Data buffer for partial sector writing */
} type_BLRecord;

typedef struct _BLRAPPCFG {
    INT32U  v_Integrity;        /*!< Application Integrity Key (CRC) binary                                 */
    PINT32U p_Base;             /*!< Application Base                                                       */
    INT32U v_Length;            /*!< Application length in words (32-bit)                                   */
    INT32U v_VectorOffset;      /*!< Vector Base Offset (Obsolete?)                                         */

    INT8U buf_version[4];           /*!< Application version stored as follows: V123 (ASCII format)             */
    INT8U buf_date[8];              /*!< Application Date stored in the following format YY_YY_MM_DD (binary)   */
    INT8U v_VersionString[64];  /*!< Application Version String in ASCII                                    */
} __attribute__((packed)) type_BLConfigApp;

// Fixed memory structure
typedef struct _BLRCFG
{
    INT32U v_ConfigVersion;     /*!< Configuration block Version, determines field types and location   */
    type_BLConfigApp st_App;    /*!< Application Region 1, configuration                                */
    INT8U buf_EmptyRecord[256];/*!< Empty record for future         */
    INT32U  v_dbf;              /*!< Fixed Value 0xDEADBEEF         */
    INT32U  v_CFGIntegrity;     /*!< Configuration (this block) CRC Value   */
} __attribute__((packed)) type_BLRCFG;

//#define p_BLRCFG      ((const type_BLRCFG *)    c_ADDR_CFGSTART)

type_BLRCFG * p_BLRCFG1;        /*!< Region 1 Application Config */
type_BLRCFG * p_BLRCFG2;        /*!< Region 2 Application Config */


// Boot Loader structure
static type_BLRecord st_BootLdr;

//------------------ Init ---------------------------------------
void f_Init_BootLoaderApp(void)
{
    // Begin from Reset State
    st_BootLdr.v_blstate = eBLStateNotReady;
    // Prepare for main state FSM
    st_BootLdr.v_MainState = eBLMainStateInit;

    // Initialize data structure
    st_BootLdr.v_CurrAddr = 0;
    st_BootLdr.v_LastSectorErased = 0;
    st_BootLdr.v_CurrLen = 0;
    st_BootLdr.v_CurrSector = 0;
    st_BootLdr.u_vecstate.st_vector.v_cfgcrc1 = '_';        /*!< CFG CRC, Config Area 1 (c/C) */
    st_BootLdr.u_vecstate.st_vector.v_cfgcrc2 = '_';        /*!< CFG CRC, Config Area 1 (c/C) */
    st_BootLdr.u_vecstate.st_vector.v_appcrc1 = '_';        /*!< Application CRC, App (a/A) */
    st_BootLdr.u_vecstate.st_vector.v_appcrc2 = '_';        /*!< Application CRC, App (a/A) */
    st_BootLdr.u_vecstate.st_vector.v_appvec1 = '_';        /*!< Application Vector 1 (_/G) */
    st_BootLdr.u_vecstate.st_vector.v_appvec2 = '_';        /*!< Application Vector 2 (_/G) */
    st_BootLdr.u_vecstate.st_vector.v_active_region = '_';  /*!< Active region (1/2) */
    
    /* Current Active Region Pointers for running */
    st_BootLdr.v_Act_appstack = 0;
    st_BootLdr.p_Act_appbase = NULL;
    st_BootLdr.v_Act_vectoroffset = 0;

    /* default configuration is hard-coded for MCF52254 */
    /* update from MCUID for other ICs with less memory like MCF52252 */
    f_BootLoaderAppHelper_GetMCUID();
}

//------------------ Begin Application/FSM ------------------------------
void f_BootLoaderApp(void)
{

    switch(st_BootLdr.v_MainState)
    {
        case eBLMainStateInit:
            st_BootLdr.v_MainState = f_BootLoaderApp_StateInit();
            break;
        case eBLMainStateChkMem:
            st_BootLdr.v_MainState = f_BootLoaderApp_StateChkMem();
            break;
        case eBLMainStateWaitCmd:
            st_BootLdr.v_MainState = f_BootLoaderApp_StateWaitCmd();
            break;
        case eBLMainStateWaitUSB:
            st_BootLdr.v_MainState = f_BootLoaderApp_StateWaitUSB();
            break;
        case eBLMainStateExecApp:
            st_BootLdr.v_MainState = f_BootLoaderApp_StateExecApp();
            break;
        case eBLMainStateWaitDnld:
            st_BootLdr.v_MainState = f_BootLoaderApp_StateWaitDnld();
            break;
        default:
            st_BootLdr.v_MainState = f_BootLoaderApp_StateInit();

    }

}
//------------------ End Application/FSM --------------------------------

//------------------ Begin States ---------------------------------------
type_BLMainState f_BootLoaderApp_StateInit(void)
{
    type_BLMainState v_state;

    v_state = eBLMainStateChkMem;

    return v_state;

}
type_BLMainState f_BootLoaderApp_StateChkMem(void)
{
    type_BLMainState v_state;

    v_state = eBLMainStateChkMem;

    // TODO: actually check for this!
    st_BootLdr.v_blstate = eBLStateIntact;
//  st_BootLdr.v_blstate = f_BootLoaderApp_SanityChk();

    if (st_BootLdr.v_blstate == eBLStateIntact)
    {
        // Memory Check Ok, move to next state
        v_state = eBLMainStateWaitCmd;
    }
    else v_state = eBLMainStateWaitDnld;  // Memory Incoherent

    return v_state;

}

type_BLMainState f_BootLoaderApp_StateWaitCmd(void)
{
    type_BLMainState v_state;

    // TODO NPYL vTaskDelay(c_TIMEWAIT_ms);

    // If timeout finished, jump to application
    v_state = eBLMainStateWaitUSB;

    // TODO:!
    // MX_USB_DEVICE_REENUM();

    return v_state;
}

type_BLMainState f_BootLoaderApp_StateWaitUSB(void)
{
    type_BLMainState v_state;

    // Wait for USB
    // TODO NPYL vTaskDelay(c_USBWAIT_ms);

    // If timeout finished, jump to application
    v_state = eBLMainStateExecApp;

    return v_state;

}

type_BLMainState f_BootLoaderApp_StateExecApp(void)
{
    type_BLMainState v_state;

    v_state = eBLMainStateExecApp;
    f_BootLoaderApp_GoApp();

    // We return to the FSM only if the above commands eFAILs.
    // However in order to be here initially checks were eSUCCESSful
    // and we should never be here.
    // In any case if we ever reach here the FSM works and with a TrapToBL command
    // BL operation will be resumed normally.
    return v_state;

}

type_BLMainState f_BootLoaderApp_StateWaitDnld(void)
{
    type_BLMainState v_state;
    INT32U v_error;

    v_state = eBLMainStateWaitDnld;


    // If a write command accepted
    if (st_BootLdr.v_blstate == eBLStateStartWr)
    {
        // revert to busy
        st_BootLdr.v_blstate = eBLStateBusy;

        // Check if sector to write is same as before (already erased)
        // Assumes a sequential programming
        // Changed for new UFW functionality:
        // Always erase if we write at first sector address

        if ((INT32U) st_BootLdr.v_CurrSector == (INT32U) st_BootLdr.v_CurrAddr)
        {
            // sector different, erase
//          v_error = f_Flash_PageErase( (INT32U) st_BootLdr.v_CurrSector);
//
//          if (v_error != HAL_FLASH_ERROR_NONE)
//          { // Sector Erase eFAILure
//              st_BootLdr.v_blstate = eBLStateError;
//          }
//          else
//          { // Sector Erase eSUCCESS
//              st_BootLdr.v_LastSectorErased = st_BootLdr.v_CurrSector;
//          }
        }

//      v_error = f_Flash_PageProg( (INT32U) st_BootLdr.v_CurrAddr, st_BootLdr.buf_datarecord, st_BootLdr.v_CurrLen );
//      if (v_error != HAL_FLASH_ERROR_NONE)
//      { // Program Error, re-erase sector
//          st_BootLdr.v_blstate = eBLStateError;
//          st_BootLdr.v_LastSectorErased = 0;
//      }
//      else
//      { // Program Ok, but surely unknown (re-check for validation)
//          st_BootLdr.v_blstate = eBLStateUnknown;
//      }
    }

    return v_state;
}


//------------------ End States ---------------------------------------



//------------------ Begin Utilities ---------------------------------------

type_BLStateEnum f_BootLoaderApp_GetCommState(void)
{
    return st_BootLdr.v_blstate;
}

type_BLStateEnum f_BootLoaderApp_GetState(void)
{
    // If state is known report back
    //if (st_BootLdr.v_blstate != eBLStateUnknown) return st_BootLdr.v_blstate;

    // Else do a small recheck - this will delay response
    // Check Application
    st_BootLdr.v_blstate = f_BootLoaderApp_SanityChk();

    return st_BootLdr.v_blstate;
}

/*!
 * @fn void f_BootLoaderApp_GetStateExt(PINT8U)
 * @brief Returns the vector state, extended version
 *
 * @param p_data : ASCII Flags, 7 bytes
 * [ (1/2) (a/A) (a/A) (c/C) (c/C) (_/G) (_/G) ]
 */
INT8U f_BootLoaderApp_GetStateExt(PINT8U p_data)
{
    INT8U v_elements = 7;

    *(p_data+0) = st_BootLdr.u_vecstate.st_vector.v_active_region;
    *(p_data+1) = st_BootLdr.u_vecstate.st_vector.v_appcrc1;
    *(p_data+2) = st_BootLdr.u_vecstate.st_vector.v_appcrc2;
    *(p_data+3) = st_BootLdr.u_vecstate.st_vector.v_cfgcrc1;
    *(p_data+4) = st_BootLdr.u_vecstate.st_vector.v_cfgcrc1;
    *(p_data+5) = st_BootLdr.u_vecstate.st_vector.v_appvec1;
    *(p_data+6) = st_BootLdr.u_vecstate.st_vector.v_appvec2;

    return v_elements;
}


INT8U f_BootLoaderApp_GetMode(void)
{

    INT8U v_retval;

    switch(st_BootLdr.v_MainState)
    {
        case eBLMainStateInit:
            v_retval = 'C';
            break;
        case eBLMainStateChkMem:
            v_retval = 'C';
            break;
        case eBLMainStateWaitCmd:
            v_retval = 'W';
            break;
        case eBLMainStateExecApp:
            v_retval = 'G';
            break;
        case eBLMainStateWaitDnld:
            v_retval = 'B';
            break;
        default:
            v_retval = 'U';

    }

    return v_retval;

}

/*!
 * @fn PINT8U f_BootLoaderApp_GetVectorState(PINT8U, PINT32, PINT32U)
 * @brief
 *
 * @todo Fix parameters FIXME
 * @param p_len
 * @param p_CPLDError
 * @param p_CPLDCount
 * @return
 */
PINT8U f_BootLoaderApp_GetVectorState(PINT8U p_len, PINT32 p_CPLDError, PINT32U p_CPLDCount)
{
    INT8U v_len;

    v_len = sizeof(st_BootLdr.u_vecstate.st_vector);
    *p_len = v_len;
    return &st_BootLdr.u_vecstate.buf_vecstate[0];
}




void f_BootLoaderApp_GoApp(void)
{
    // check if CRC/chksum is correct, if not set Application Incoherent

    // check if Reset Vector instruction is jsr
    // check if jsr argument is valid (lies between valid address space)
    // This check is superset of NULL or 0xFFFFFFFF check of pointer.
    // Pointer should lies above 65K and below 65K+128K.
    // If there is a eFAILure skip jump to application
    // and set Vector Error
    PINT32U p_appbase;
    INT32U v_appstack;
    INT32U v_vectoroffset;


    switch(st_BootLdr.u_vecstate.st_vector.v_active_region)
    {
        case '1':
            p_appbase = p_BLRCFG1->st_App.p_Base;
            v_vectoroffset = p_BLRCFG1->st_App.v_VectorOffset;
            break;
        case '2':
            p_appbase = p_BLRCFG2->st_App.p_Base;
            v_vectoroffset = p_BLRCFG2->st_App.v_VectorOffset;
            break;
        default: /* we should not be here in the first place, do nothing */
            st_BootLdr.v_MainState = eBLMainStateInit;
            f_Init_BootLoaderApp();
            return;
    }


    if ( f_BootLoaderApp_GetState() == eBLStateIntact)
    {

        v_appstack = *(p_appbase+0);

        st_BootLdr.p_Act_appbase = p_appbase;
        st_BootLdr.v_Act_appstack = v_appstack;
        st_BootLdr.v_Act_vectoroffset = v_vectoroffset;

        f_BootLoaderApp_HelperGoApp();


    }

}

void f_BootLoaderApp_HelperGoApp(void)
{
}



/*!
 * \brief Aborts ExecToApp
 * \details Is usually called to abort execution in case the Application is Intact and thus remain in BL mode
 * \detail Function to be called from NetWorkL2Action AND as a workaround for ufw bug on L2C BL on Write Sector.
 *
 */
void f_BootLoaderApp_AbortExec(void)
{
    if (st_BootLdr.v_MainState == eBLMainStateWaitCmd) st_BootLdr.v_MainState = eBLMainStateWaitDnld;
}

status_t f_BootLoaderApp_ChkBlockBlank(PINT32U p_addr, INT32U v_bytelen)
{
    status_t v_result;
    INT32U v_idx;

    v_result = eSUCCESS;

    for(v_idx=0; v_idx< (v_bytelen >> 2); v_idx++)
    {
        if (*(p_addr+v_idx) != 0xFFFFFFFF)
        {
            v_result = eFAIL;
            break;
        }
    }

    return v_result;
}
//------------------ End Utilities ---------------------------------------


//------------------ Begin Area Checks  ---------------------------------------
type_BLStateEnum f_BootLoaderApp_SanityChk(void)
{
    type_BLStateEnum v_retcode;
    status_t v_result1, v_result2;

    // default initial
    v_retcode = eBLStateIntact;

    // Check Config Block
    v_result1 = f_BootLoaderApp_HelperChkSection1();
    v_result2 = f_BootLoaderApp_HelperChkSection2();
    v_retcode = f_BootLoaderApp_HelperDetermineActiveRegion(v_result1, v_result2);


    return v_retcode;
}

type_BLStateEnum f_BootLoaderApp_HelperDetermineActiveRegion(status_t v_Region1, status_t v_Region2)
{
    type_BLStateEnum v_retcode;
    INT8U v_result;

    v_retcode = eBLStateIntact;

    if ( (v_Region1 == eFAIL) && (v_Region2 == eFAIL))
    {
        /* incoherent state for all regions, just update flags */
        v_retcode = eBLStateIncoherent;
    }
    else if ( (v_Region1 == eSUCCESS) && (v_Region2 == eSUCCESS))
    {
        /* Determine Active Region */
        v_result = f_UtilsCmpBuffer8(&p_BLRCFG1->st_App.buf_version[0], &p_BLRCFG2->st_App.buf_version[0], 4);

        if (v_result == 1)
        {
            /* V1 > V2, Region 1 Active */
            f_BootLoaderApp_HelperSetRegion1();
        }
        else if (v_result == 0)
        {
            /* V1 = V2, check if dates are different */
            // TODO: CMP
            v_result = f_UtilsCmpBuffer8(&p_BLRCFG1->st_App.buf_date[0], &p_BLRCFG2->st_App.buf_date[0], 8);

            if (v_result == 2)
            {   /* Date 1 < Date 2, Region 2 Active */
                f_BootLoaderApp_HelperSetRegion2();
            }
            else
            {   /* All other cases (Date 1 >= Date 2), Region 1 active */
                f_BootLoaderApp_HelperSetRegion1();
            }
        }
        else
        { /* V2 > V1, Region 2 Active */
            f_BootLoaderApp_HelperSetRegion2();
        }

    }
    else if (v_Region1 == eSUCCESS)
    {
        /* Region 1 Active */
        f_BootLoaderApp_HelperSetRegion1();
    }
    else  /* (v_Region2 == eSUCCESS) */
    {
        /* Region 2 Active */
        f_BootLoaderApp_HelperSetRegion2();
    }


    return v_retcode;
}

void f_BootLoaderApp_HelperSetRegion1(void)
{
    /* Region 1 Active */
    st_BootLdr.v_AppStart = FLASH_ADDRESS_APP1_MIN;
    st_BootLdr.v_AppStop = FLASH_ADDRESS_APP1_MAX;
    st_BootLdr.v_CfgStart = FLASH_ADDRESS_CFG1_MIN;
    st_BootLdr.v_CfgStop = FLASH_ADDRESS_CFG1_MAX;
    st_BootLdr.u_vecstate.st_vector.v_active_region = '1';
}

void f_BootLoaderApp_HelperSetRegion2(void)
{
    /* Region 2 Active */
    st_BootLdr.v_AppStart = FLASH_ADDRESS_APP2_MIN;
    st_BootLdr.v_AppStop = FLASH_ADDRESS_APP2_MAX;
    st_BootLdr.v_CfgStart = FLASH_ADDRESS_CFG2_MIN;
    st_BootLdr.v_CfgStop = FLASH_ADDRESS_CFG2_MAX;
    st_BootLdr.u_vecstate.st_vector.v_active_region = '2';
}

/*!
 * @fn status_t f_BootLoaderApp_HelperChkSection1(void)
 * @brief Performs all checks for section 1
 * \details Checks Config Blank-CRC, App Blank-CRC, Vectors Valid
 *
 * @return eSUCCESS: Section is valid, eFAIL: Section is invalid
 */
status_t f_BootLoaderApp_HelperChkSection1(void)
{
    status_t v_result;
    status_t v_result1, v_result2, v_result3;

    v_result1 = f_BootLoaderApp_HelperChkBlockCfg1();


    if (v_result1 == eSUCCESS)
    {
        v_result2 = f_BootLoaderApp_HelperChkBlockApp1();
        v_result3 = f_BootLoaderApp_HelperChkVectorApp1();
    }

    if (  (v_result1 == eSUCCESS) && (v_result2 == eSUCCESS) && (v_result3 == eSUCCESS))
    {
        /* Block one is valid */
        v_result = eSUCCESS;
    }
    else
    {
        v_result = eFAIL;
    }

    return v_result;
}

/*!
 * @fn status_t f_BootLoaderApp_HelperChkSection2(void)
 * @brief Performs all checks for section 2
 * \details Checks Config Blank-CRC, App Blank-CRC, Vectors Valid
 *
 * @return eSUCCESS: Section is valid, eFAIL: Section is invalid
 */
status_t f_BootLoaderApp_HelperChkSection2(void)
{
    status_t v_result;
    status_t v_result1, v_result2, v_result3;

    v_result1 = f_BootLoaderApp_HelperChkBlockCfg2();

    if (v_result1 == eSUCCESS)
    {
        v_result2 = f_BootLoaderApp_HelperChkBlockApp2();
        v_result3 = f_BootLoaderApp_HelperChkVectorApp2();
    }

    if (  (v_result1 == eSUCCESS) && (v_result2 == eSUCCESS) && (v_result3 == eSUCCESS))
    {
        /* Block one is valid */
        v_result = eSUCCESS;
    }
    else
    {
        v_result = eFAIL;
    }


    return v_result;
}

status_t f_BootLoaderApp_HelperChkBlockCfg1(void)
{
    status_t v_result;
    PINT8U p_data;
    INT32U v_len;
    INT32U v_crc;

    p_data = (PINT8U) (FLASH_ADDRESS_CFG1_MIN);
    v_len = sizeof(type_BLRCFG)-4;;
    v_crc = p_BLRCFG1->v_CFGIntegrity;
    v_result = f_BootLoaderApp_HelperChkBase(p_data, v_len, v_crc);

    if (v_result == eFAIL)
    {
        st_BootLdr.u_vecstate.st_vector.v_cfgcrc1 = 'c';
    }
    else st_BootLdr.u_vecstate.st_vector.v_cfgcrc1 = 'C';


    return v_result;
}

status_t f_BootLoaderApp_HelperChkBlockCfg2(void)
{
    status_t v_result;
    PINT8U p_data;
    INT32U v_len;
    INT32U v_crc;

    p_data = (PINT8U) (FLASH_ADDRESS_CFG2_MIN);
    v_len = sizeof(type_BLRCFG)-4;;
    v_crc = p_BLRCFG2->v_CFGIntegrity;
    v_result = f_BootLoaderApp_HelperChkBase(p_data, v_len, v_crc);

    if (v_result == eFAIL)
    {
        st_BootLdr.u_vecstate.st_vector.v_cfgcrc2 = 'c';
    }
    else st_BootLdr.u_vecstate.st_vector.v_cfgcrc2 = 'C';


    return v_result;
}


// Check CRC of Application Block
// TODO: Add Valid Address Space Check (Added, but needs further checks later for length)
// TODO: Refactor to have a single function for both 1&2 regions
status_t f_BootLoaderApp_HelperChkBlockApp1(void)
{
    status_t v_result;
    PINT8U p_data;
    INT32U v_len;
    INT32U v_crc;

    p_data = (PINT8U) (p_BLRCFG1->st_App.p_Base);
    p_data = (PINT8U) f_BootLoaderApp_ValidateAddress( (INT32U) p_data);
    if (p_data == 0) return eFAIL;

    v_len = p_BLRCFG1->st_App.v_Length;
    v_crc = p_BLRCFG1->st_App.v_Integrity;
    v_result = f_BootLoaderApp_HelperChkBase(p_data, v_len, v_crc);

    if (v_result == eFAIL)
    {
        st_BootLdr.u_vecstate.st_vector.v_appcrc1 = 'a';
    }
    else st_BootLdr.u_vecstate.st_vector.v_appcrc1 = 'A';

    return v_result;
}

// Check CRC of Application Block
// TODO: Add Valid Address Space Check (Added, but needs further checks later for length)
// TODO: Refactor to have a single function for both 1&2 regions
status_t f_BootLoaderApp_HelperChkBlockApp2(void)
{
    status_t v_result;
    PINT8U p_data;
    INT32U v_len;
    INT32U v_crc;

    p_data = (PINT8U) (p_BLRCFG2->st_App.p_Base);
    p_data = (PINT8U) f_BootLoaderApp_ValidateAddress( (INT32U) p_data);
    if (p_data == 0) return eFAIL;

    v_len = p_BLRCFG2->st_App.v_Length;
    v_crc = p_BLRCFG2->st_App.v_Integrity;
    v_result = f_BootLoaderApp_HelperChkBase(p_data, v_len, v_crc);

    if (v_result == eFAIL)
    {
        st_BootLdr.u_vecstate.st_vector.v_appcrc2 = 'a';
    }
    else st_BootLdr.u_vecstate.st_vector.v_appcrc2 = 'A';

    return v_result;
}



/*!
 * @fn status_t f_BootLoaderApp_HelperChkVectorApp1(void)
 * @brief Validate Reset vector of Application1
 * @details Checks that Application base is within Application 1 space
 * @details Checks that *(Application base) (content) is within Application 1 space
 * @details Checks that Vector Offset is within Application 1 space
 * @return eSUCCESS: all checks passed, eFAIL: Failed check
 */
status_t f_BootLoaderApp_HelperChkVectorApp1(void)
{
    status_t v_result;
    INT32U UserEntryCheck;

    st_BootLdr.v_ChkVectorStatus = eCHKJSR;


    st_BootLdr.u_vecstate.st_vector.v_appvec1 = 'g';

    /* check there is a valid memory location > minimum */
    st_BootLdr.v_ChkVectorStatus = eCHKLow;
    UserEntryCheck = (INT32U) p_BLRCFG1->st_App.p_Base;
    if(UserEntryCheck < FLASH_ADDRESS_APP1_MIN) return eFAIL;
    /* check there is a valid jump vector (Lower Application Area) */
    UserEntryCheck += 4;
    if(*((PINT32U) UserEntryCheck) < FLASH_ADDRESS_APP1_MIN) return eFAIL;

    /* Check Vector Jump Table Offset is within Application Space */
    UserEntryCheck = p_BLRCFG1->st_App.v_VectorOffset;
    if(UserEntryCheck < FLASH_ADDRESS_APP1_MIN) return eFAIL;
    st_BootLdr.v_ChkVectorStatus = eCHKHi;
    if(UserEntryCheck > FLASH_ADDRESS_APP1_MAX) return eFAIL;

    /* check there is a valid jump vector (Higher Application Area) */
    UserEntryCheck = (INT32U) p_BLRCFG1->st_App.p_Base;
    if(UserEntryCheck > FLASH_ADDRESS_APP1_MAX) return eFAIL;
    /* check there is a valid jump vector (Higher Application Area) */
    UserEntryCheck += 4;
    if(*((PINT32U) UserEntryCheck) > FLASH_ADDRESS_APP1_MAX) return eFAIL;

    /* this is not required, as it is already covered by the previous test */
    /* check there is a valid jump vector (Blank Check in application vector map) */
    st_BootLdr.v_ChkVectorStatus = eCHKBLK;
    if(*((PINT32U) UserEntryCheck) == 0xFFFFFFFF) return eFAIL;

    st_BootLdr.v_ChkVectorStatus = eCHKOK;
    v_result = eSUCCESS;
    st_BootLdr.u_vecstate.st_vector.v_appvec1 = 'G';

    return v_result;
}

/*!
 * @fn status_t f_BootLoaderApp_HelperChkVectorApp2(void)
 * @brief Validate Reset vector of Application2
 * @details Checks that Application base is within Application 2 space
 * @details Checks that *(Application base) (content) is within Application 2 space
 * @details Checks that Vector Offset is within Application 2 space
 * @return eSUCCESS: all checks passed, eFAIL: Failed check
 */
status_t f_BootLoaderApp_HelperChkVectorApp2(void)
{
    status_t v_result;
    PINT32U UserEntryCheck;

    st_BootLdr.v_ChkVectorStatus = eCHKJSR;


    st_BootLdr.u_vecstate.st_vector.v_appvec2 = 'g';

    /* check there is a valid memory location > minimum */
    st_BootLdr.v_ChkVectorStatus = eCHKLow;
    UserEntryCheck = p_BLRCFG2->st_App.p_Base;
    if((INT32U) UserEntryCheck < FLASH_ADDRESS_APP2_MIN) return eFAIL;
    /* check there is a valid jump vector (Lower Application Area) */
    if(*(UserEntryCheck+1) < FLASH_ADDRESS_APP2_MIN) return eFAIL;

    /* Check Vector Jump Table Offset is within Application Space */
    UserEntryCheck = (PINT32U) p_BLRCFG2->st_App.v_VectorOffset;
    if((INT32U) UserEntryCheck < FLASH_ADDRESS_APP2_MIN) return eFAIL;
    st_BootLdr.v_ChkVectorStatus = eCHKHi;
    if((INT32U) UserEntryCheck > FLASH_ADDRESS_APP2_MAX) return eFAIL;

    /* check there is a valid jump vector (Higher Application Area) */
    UserEntryCheck = p_BLRCFG2->st_App.p_Base;
    if((INT32U) UserEntryCheck > FLASH_ADDRESS_APP2_MAX) return eFAIL;
    /* check there is a valid jump vector (Higher Application Area) */
    if(*(UserEntryCheck+1) > FLASH_ADDRESS_APP2_MAX) return eFAIL;

    /* this is not required, as it is already covered by the previous test */
    /* check there is a valid jump vector (Blank Check in application vector map) */
    st_BootLdr.v_ChkVectorStatus = eCHKBLK;
    if(*(UserEntryCheck+1) == 0xFFFFFFFF) return eFAIL;

    st_BootLdr.v_ChkVectorStatus = eCHKOK;
    v_result = eSUCCESS;
    st_BootLdr.u_vecstate.st_vector.v_appvec2 = 'G';

    return v_result;
}



status_t f_BootLoaderApp_HelperChkBase(PINT8U p_data, INT32U v_len, INT32U v_ExpectedCRC)
{
    status_t v_result;
    INT32U v_crc;

    v_result = eSUCCESS;

    v_crc = 0xFFFFFFFF;

    // Blank Check
    v_result = f_BootLoaderApp_ChkBlockBlank((PINT32U) p_data, v_len);
    if (v_result == eSUCCESS) return eFAIL;

    // CRC Check
    // TODO: CRC
//  v_crc = f_CRCCompute(p_data, v_len, v_crc);

    if (v_crc != v_ExpectedCRC) v_result = eFAIL;
    else v_result = eSUCCESS;

    return v_result;
}

/*!
 * @fn status_t f_BootLoaderApp_HelperChkVector(PINT8U, INT32U, INT32U)
 * @brief checks if the address vector is within the memory range
 * \deprecated Not used as we do this in combined tests and provide flags accordingly
 * @param p_vector : Address to test
 * @param v_LowAddr : Lower bound
 * @param v_HighAddr : High bound
 * @return eSUCCESS: p_vector is within region, eFAIL: p_vector is outside the region
 */
status_t f_BootLoaderApp_HelperChkVector(PINT8U p_vector, INT32U v_LowAddr, INT32U v_HighAddr)
{
    status_t v_result;
    INT32U v_Vector;

    v_result = eSUCCESS;

    v_Vector = (INT32U) p_vector;
    // check there is a valid jump vector (Blank Check)
    if(v_Vector == 0xFFFFFFFF) return eFAIL;
    // check there is a valid jump vector (Lower Application Area)
    if(v_Vector > v_HighAddr) return eFAIL;
    // check there is a valid jump vector (Higher Application Area)
    if(v_Vector < v_LowAddr) return eFAIL;

    return v_result;
}


//------------------ End Area Checks  ---------------------------------------


//------------------ Begin Activity Check ---------------------------------
INT8U f_BooLoaderApp_CheckActity(void)
{
    INT8U v_state;

    v_state = 0;

    switch (st_BootLdr.v_blstate)
    {
        case eBLStateBusy:
        case eBLStateStartEr:
            v_state = 1;
            break;
        case eBLStateStartWr:
            v_state = 2;
            break;
        default:;
    }

    return v_state;
}
//------------------ End Activity Check  ---------------------------------------

//------------------ Begin COMMCMD Write Record ---------------------------------
void f_BootLoaderApp_WriteStart(PINT8U p_addr, PINT8U p_bin_data, INT8U v_len)
{
    INT8U v_idx;
    INT32U v_newsector;
    INT8U v_state;

    // If we already in busy mode discard any input data
    // if state is eBLStateStartWr, no write begun, we can override.

    v_state = f_BooLoaderApp_CheckActity();
    if (v_state == 1) return;

    /* If address required is below BootLoader Address space, abort!
       BootLoader cannot be overwitten directly. */
    /* Checked on upper layer */
    //if ( (INT32U) p_addr < c_ADDR_BLRSTOP) return;
    /* Checked on upper layer */
    /* address should be within memory limits */
    //if ( (INT32U) p_addr > (c_ADDR_RSVSTOP-c_DATARECORDLEN) ) return;

    if ( v_len > c_DATARECORDLEN ) return;

    /* in case we are not already in WaitDownload, enter this mode
     * Used as a workaround for UFW which in L2C mode does not issue TrapBL twice.
     */
    f_BootLoaderApp_AbortExec();

    // We are able to program, initialize record address & sectors
    st_BootLdr.v_CurrAddr = p_addr;
    v_newsector = (INT32U) p_addr & ~(c_PAGE_SIZE_Bytes-1);
    st_BootLdr.v_CurrSector = v_newsector;
    st_BootLdr.v_CurrLen = v_len;

    // Convert and copy actual RX data to record buffer
    for(v_idx=0;v_idx<64;v_idx++)
    {
        st_BootLdr.buf_datarecord[v_idx] = *(p_bin_data+v_idx);
    }

#if 0
    /* signal main application to begin record of data */
    if ((INT32U) st_BootLdr.v_CurrSector == (INT32U) st_BootLdr.v_CurrAddr)
    {   /* Sector match, we need to erase first */
        st_BootLdr.v_blstate = eBLStateStartEr;

        v_error = f_Flash_PageErase_NonBlock( (INT32U) st_BootLdr.v_CurrSector);

    }
    else
    {   /* Sector continue, only program */
        st_BootLdr.v_blstate = eBLStateStartWr; //eBLStateStartEr
    }
#else
    st_BootLdr.v_blstate = eBLStateStartWr;
#endif
}
//------------------ End COMMCMD Write Record ---------------------------------

//------------------ Start Utility Function for Supporting Different Chip Configurations  ----------
void f_BootLoaderAppHelper_GetMCUID(void)
{
    /* Default Configuration Space */
    st_BootLdr.v_CfgStart = FLASH_ADDRESS_CFG1_MIN;
    st_BootLdr.v_CfgStop = FLASH_ADDRESS_CFG2_MAX;

    //p_BLRCFG1 = ((type_BLRCFG *)    st_BootLdr.v_CfgStart);
    /* Map configuration structures to real flash memory */
    p_BLRCFG1 = ((type_BLRCFG *)    (FLASH_ADDRESS_CFG1_MIN) );
    p_BLRCFG2 = ((type_BLRCFG *)    (FLASH_ADDRESS_CFG2_MIN) );
}

/*!
 * \brief provide application available space
 * \details based on chip identification
 * \details used on providing data to the host from Network layer
 */
INT32U f_BootLoaderApp_GetAppSpaceLen(void)
{
    INT32U v_applen;

    v_applen = st_BootLdr.v_CfgStop - FLASH_ADDRESS_APP1_MIN + 1;

    return v_applen;
}


/*!
 * \brief Validates given address to be within valid limits
 * \details address should be in Application Space
 * \details address should be within memory limits 
 * \param v_addr : Input Address
 * \return 0 for invalid address, intact input (pass-through) for valid address
 */
INT32U f_BootLoaderApp_ValidateAddress(INT32U v_Address)
{
    INT32U v_addrOut;

    if((v_Address >= FLASH_ADDRESS_APP1_MIN) && (v_Address <= FLASH_ADDRESS_APP1_MAX))
        v_addrOut = v_Address;
    else if ((v_Address >= FLASH_ADDRESS_APP2_MIN) && (v_Address <= FLASH_ADDRESS_APP2_MAX))
        v_addrOut = v_Address;
    else if ((v_Address >= FLASH_ADDRESS_CFG1_MIN) && (v_Address <= FLASH_ADDRESS_CFG1_MAX))
        v_addrOut = v_Address;
    else if ((v_Address >= FLASH_ADDRESS_CFG2_MIN) && (v_Address <= FLASH_ADDRESS_CFG2_MAX))
        v_addrOut = v_Address;
    else /* Address ERROR, FAIL */
    {
        v_addrOut = 0;
    }

    return v_addrOut;
}

/*!
 * @fn type_bool f_BootLoaderApp_ValidateAddressRegion(INT32U, INT8U)
 * @brief Check if the provided address lies within (Cfg or App) the selected region (1 or 2)
 *
 * @param v_addr: Address to check
 * @param v_region: Region that the address has to be
 * @return bTRUE: Address within App or Cfg sections, bFALSE: Address outside App or Cfg sections for this region
 */
type_bool f_BootLoaderApp_ValidateAddressRegion(INT32U v_addr, INT8U v_region)
{

    if (v_region == 1)
    {
        if((v_addr >= FLASH_ADDRESS_APP1_MIN) && (v_addr <= FLASH_ADDRESS_APP1_MAX))
            return bTRUE;
        else if ((v_addr >= FLASH_ADDRESS_CFG1_MIN) && (v_addr <= FLASH_ADDRESS_CFG1_MAX))
            return bTRUE;
        else
            return bFALSE;
    }
    else
    {
        if ((v_addr >= FLASH_ADDRESS_APP2_MIN) && (v_addr <= FLASH_ADDRESS_APP2_MAX))
            return bTRUE;
        else if ((v_addr >= FLASH_ADDRESS_CFG2_MIN) && (v_addr <= FLASH_ADDRESS_CFG2_MAX))
            return bTRUE;
        else
            return bFALSE;
    }

}


/*!
 * @fn type_bool f_BootLoaderApp_ValidateAddressRegionApp(INT32U, INT8U)
 * @brief Check if the provided address lies within (Cfg or App) the selected region (1 or 2)
 *
 * @param v_addr: Address to check
 * @param v_region: Region that the address has to be
 * @return bTRUE: Address within App section, bFALSE: Address outside App section for this region
 */
type_bool f_BootLoaderApp_ValidateAddressRegionApp(INT32U v_addr, INT8U v_region)
{

    if (v_region == 1)
    {
        if((v_addr >= FLASH_ADDRESS_APP1_MIN) && (v_addr <= FLASH_ADDRESS_APP1_MAX))
            return bTRUE;
        else
            return bFALSE;
    }
    else
    {
        if ((v_addr >= FLASH_ADDRESS_APP2_MIN) && (v_addr <= FLASH_ADDRESS_APP2_MAX))
            return bTRUE;
        else
            return bFALSE;
    }

}


//------------------ End Utility Function for Supporting Different Chip Configurations  ----------
