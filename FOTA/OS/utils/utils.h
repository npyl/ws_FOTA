/* ********************** Utils.h ***********************************
    Title:    		Utils.h
	Project:		L2CV300TDD
    Author:   		Alexopoulos Ilias
    Company:		Intralot S.A.
    Date:     		09 ��� 2012
    Purpose:
    Configuration: 	CodeWarrior MCF 10.2
    Last updated:
    CPU: 			PIC16F6627

    Version     Date            Description
    V0.00       09 ��� 2012    	Creation


************************************************************************* */


#ifndef UTILS_H_
#define UTILS_H_

typedef enum {
	eL1RxOk,			/*!< L1 Packet Received ok */
	eL1RxChar,			/*!< L1 Packet contains non ASCII chars */
	eL1ChkSum,			/*!< L1 Packet had bad checksum */
	eL1FrmErr			/*!< L1 Packet had bad format */
} type_L1PacketCheck;

INT16U f_UtilsCRCInit(void);
INT16U f_UtilsCRCSingleCalc(INT16U crc, INT8U data);
INT16U f_UtilsCRCFinal(INT16U crc);

INT16U f_UtilsCRCBlockCalc(const INT8U *data, INT16U len);



INT8U f_UtilsGeneralASCIICheckOk(PINT8U p_buffer, INT8U v_len);
INT8U f_UtilsHexASCIICheckOk(PINT8U p_buffer, INT8U v_len);
INT8U f_UtilsL3ChkEscChar(PINT8U p_data);
INT16U f_UtilsL3ComputeCRC(PINT8U p_buffer, INT8U v_len);
INT16U f_UtilsL3CopyConvert(PINT8U p_srcbuffer, PINT8U p_dstbuffer);
INT8U f_UtilsL1ComputeChecksum(PINT8U p_buffer, INT8U v_len);
void f_UtilsL1TxPlaceChecksum(PINT8U p_buffer);
type_L1PacketCheck f_UtilsL1RxPacketCheck(PINT8U p_buffer);

void f_UtilsTypeConv_BinHex8(INT8U bin, INT8 *str);
void f_UtilsTypeConv_BinHex16(INT16U v_bin, INT8 *str);

INT8U f_UtilsTypeConv_HexBin8(INT8U *hex);
INT16U f_UtilsTypeConv_HexBin16(INT8U *hex);
INT32U f_UtilsTypeConv_HexBin32(INT8U *hex);

void f_UtilsBufferCopyRam2Ram(PINT8U p_src, PINT8U p_dest, INT8U v_len);
void f_UtilsBufferCopyRom2Ram(const INT8U *p_src, PINT8U p_dest, INT8U v_len);
INT8U f_UtilsCopyString(PINT8U p_src, PINT8U p_dst, INT8U v_maxlen);
INT16U f_UtilsStrLen(PINT8U p_src);

INT8U f_UtilsFindL3CmdIdx(PINT16U p_lut, INT8U v_len, INT16U v_element);
INT8U f_UtilsCmpBuffer16(INT16U *p_str1, INT16U *p_str2, INT16U v_len);
INT8U f_UtilsCmpBuffer8(INT8U *p_str1, INT8U *p_str2, INT16U v_len);

INT16 f_UtilsAbs(INT16 v_signed);

#ifdef c_UTEST_HOST
INT16U f_UtilsL3ComputeCRCTDD(PINT8U p_buffer, INT8U v_len);

#endif

INT32U f_CRCCompute(PINT8U p_base, INT32U v_length, INT32U v_currcrc);
void f_CRCGenLUT(INT8U v_len);

INT16U f_Util_CRC16(PINT16U p_start, PINT16U p_end);
INT8U f_Util_CRC8(INT8U v_new, INT8U v_currcrc);


#endif /* UTILS_H_ */
