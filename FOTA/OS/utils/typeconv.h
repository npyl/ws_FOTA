void f_TypeConv_BinHex(INT8U bin, INT8 *str);
INT8U f_TypeConvASCIICheckLong(INT8U *pasciilong);
INT8U f_TypeConvASCIICheckByte(INT8U *p_asccibyte);
void f_TypeConv_BinHex16(INT16U v_bin, INT8 *str);
INT32U f_TypeConv_HexBin32(INT8U *hex);
INT16U f_TypeConv_HexBin(INT8U *hex);
INT8U f_TypeConv_HexBinByte(INT8U *hex);
INT8U *f_TypeConv_CopyString(INT8U *p_str1, INT8U *p_output);
void f_TypeConv_Dec2Bin(INT32 decimal, INT8U *binary);
INT8U f_TypeConv_CmpString(INT8U *p_str1, INT8U *p_str2);
INT8U f_TypeConv_CmpBuffer16(INT16U *p_str1, INT16U *p_str2, INT16U v_len);
INT16U f_TypeConv_StringLen(INT8U *p_str1);
INT32 f_TypeConv_Ascii2Int(INT8* p_string);
INT32 f_TypeConv_Exp(INT8U v_base, INT8U v_exp);


