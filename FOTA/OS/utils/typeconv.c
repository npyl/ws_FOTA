#include "StdTypes.h"
#include "settings.h"

#include "typeconv.h"

// Convert INT8U to Hex string
void f_TypeConv_BinHex(INT8U bin, INT8 *str)
{
	const INT8 lut_Hex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	*(str+0) = lut_Hex[(bin >> 4) & 0x0f];
	*(str+1) = lut_Hex[bin & 0x0f];
}

INT8U f_TypeConvASCIICheckLong(INT8U *pasciilong)
{
	INT8U v_success;
	INT8U v_retbyte;
	
	v_success = 0;
	
	v_retbyte = f_TypeConvASCIICheckByte(pasciilong+0);
	v_success += v_retbyte;
	v_retbyte = f_TypeConvASCIICheckByte(pasciilong+2);
	v_success += v_retbyte;
	v_retbyte = f_TypeConvASCIICheckByte(pasciilong+4);
	v_success += v_retbyte;
	v_retbyte = f_TypeConvASCIICheckByte(pasciilong+6);
	v_success += v_retbyte;
		
		
	if (v_success == 4) v_success = 1;
	else v_success = 0;		
	
	return v_success;
	
}

INT8U f_TypeConvASCIICheckByte(INT8U *p_asccibyte)
{
	INT8U v_idx;
	INT8U v_success;
	const INT8 lut_Hex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	
	v_success = 0;
	// check first nibble
	// if match then increment status
	for(v_idx=0;v_idx<16;v_idx++)
	{
		if (*p_asccibyte == lut_Hex[v_idx]) v_success++;
	}
	
	// get next nibble
	p_asccibyte++;
	// if match then increment status
	for(v_idx=0;v_idx<16;v_idx++)
	{
		if (*p_asccibyte == lut_Hex[v_idx]) v_success++;
	}
	// if all nibbles are valid then status have increased twice	
	if (v_success == 2) v_success = 1;
	else v_success = 0;		
	
	return v_success;
}

// Convert INT16U to Hex string
void f_TypeConv_BinHex16(INT16U v_bin, INT8 *str)
{
	INT8U v_byte;
	INT8 *p_mystr;
	
	p_mystr = str;
	v_byte = (INT8U) (v_bin & 0xff);
	f_TypeConv_BinHex(v_byte, p_mystr);
	v_byte = (INT8U) ((v_bin >> 8) & 0xff);
	p_mystr += 2;
	f_TypeConv_BinHex(v_byte, p_mystr);
}

// Convert Hex String to INT32U -- 8 digits
INT32U f_TypeConv_HexBin32(INT8U *hex)
{
	INT32U v_data;
	INT8U i;
	INT8U cbyte;

	v_data = 0L;
	
	for(i=0; i<8; i++)
	{
		cbyte = *(hex+i);
		if ( cbyte >= 'A') cbyte = (INT8U) ((cbyte - 'A') + 10);
		else cbyte-= '0';
		v_data |= (INT32U) (cbyte << (4*(7-i)));
	}

	return v_data;
}


// Convert Hex String to INT16U -- 4 digits
INT16U f_TypeConv_HexBin(INT8U *hex)
{
	INT16U data = 0;
	INT8U i;
	INT8U cbyte;

	for(i=0; i<4; i++)
	{
		cbyte = *(hex+i);
		if ( cbyte >= 'A') cbyte = (INT8U) ((cbyte - 'A') + 10);
		else cbyte-= '0';
		data |= (cbyte << (4*(3-i)));
	}

	return data;
}

// Convert Hex String to INT8U -- 2 digits
INT8U f_TypeConv_HexBinByte(INT8U *hex)
{
	INT8U data = 0;
	INT8U i;
	INT8U cbyte;

	for(i=0; i<2; i++)
	{
		cbyte = *(hex+i);
		if ( cbyte >= 'A') cbyte = (INT8U) ((cbyte - 'A') + 10);
		else cbyte-= '0';
		data |= (cbyte << (4*(1-i)));
	}

	return data;
}


INT8U * f_TypeConv_CopyString(INT8U *p_str1, INT8U *p_output)
{
	INT8U *p_srcstr;
	INT8U *p_dststr;
	
	p_srcstr = p_str1;
	p_dststr = p_output; 
	
	// If pointer empty then return
	if (p_srcstr == NULL) return p_dststr;
	if (p_dststr == NULL) return p_dststr;
	
	while(*p_srcstr)
	{
		*p_dststr = *p_srcstr;
		p_srcstr++;
		p_dststr++;
	}
	
	return p_dststr;
}

INT8U f_TypeConv_CmpString(INT8U *p_str1, INT8U *p_str2)
{
	INT8U *p_srcstr;
	INT8U *p_dststr;
    INT8U v_idx;
	
	p_srcstr = p_str1;
	p_dststr = p_str2; 
	    
	// If pointer empty then return
	if (p_srcstr == NULL) return 0;
	if (p_dststr == NULL) return 0;
	
    v_idx = 0;
    
	while(*p_srcstr)
	{
		if ( *p_dststr != *p_srcstr) return 0;
		p_srcstr++;
		p_dststr++;
        v_idx++;
	}
	
    // check for NULL string input
    if (v_idx == 0) return 0;
    
    // Else indeed strings are same
	return 1;
}
// Compare two buffers 16-bit
INT8U f_TypeConv_CmpBuffer16(INT16U *p_str1, INT16U *p_str2, INT16U v_len)
{
	INT16U *p_srcstr;
	INT16U *p_dststr;
    INT16U v_idx;
	
	p_srcstr = p_str1;
	p_dststr = p_str2; 
	    
	// If pointer empty then return
	if (p_srcstr == NULL) return 0;
	if (p_dststr == NULL) return 0;
	
    v_idx = 0;
    
	for(v_idx=0; v_idx<v_len; v_idx++)
	{
		if ( *p_dststr != *p_srcstr) return 0;
		p_srcstr++;
		p_dststr++;
	}
	
    // check for NULL string input
    if (v_idx == 0) return 0;
    
    // Else indeed strings are same
	return 1;
}

INT16U f_TypeConv_StringLen(INT8U *p_str1)
{
	INT8U *p_srcstr;	
    INT16U v_idx;
	
	p_srcstr = p_str1;
	    
	// If pointer empty then return
	if (p_srcstr == NULL) return 0;
	
    v_idx = 0;
    
	while(*p_srcstr)
	{
		p_srcstr++;		
        v_idx++;
	}
	
    // Else indeed strings are same
	return v_idx;
}

//
// accepts a decimal integer and returns a binary coded string
//
void f_TypeConv_Dec2Bin(INT32 decimal, INT8U *binary)
{
    int k = 0, n = 0;
    int neg_flag = 0;
    int remain;
    INT8U temp[80];
    
    // take care of negative input
    
    if (decimal < 0)
    {
        decimal = -decimal;
        neg_flag = 1;
    }
    
    do
    {
        remain = decimal % 2;
        // whittle down the decimal number
        decimal = decimal / 2;
        // this is a test to show the action
        //printf("%d/2 = %d remainder = %d\n", old_decimal, decimal, remain);
        // converts digit 0 or 1 to character '0' or '1'
        temp[k++] = (unsigned char) (remain + '0');
    } while (decimal > 0);
 
    if (neg_flag)
        temp[k++] = '-'; // add - sign
    else
    temp[k++] = ' '; // space

    // reverse the spelling
    while (k >= 0)
        binary[n++] = temp[--k];
    
    binary[n-1] = 0; // end with NULL
}

INT32 f_TypeConv_Exp(INT8U v_base, INT8U v_exp)
{
	INT8U v_idx;
	INT32 v_result;
	
	v_result = 1;
	
	for(v_idx=0; v_idx < v_exp; v_idx++)
	{
		v_result *= v_base;
	}
	
	return v_result;
}

INT32 f_TypeConv_Ascii2Int(INT8* p_string)
{
	INT32 v_result;
	INT8 *p_char;
	INT8U v_idx;
	INT8U v_maxidx;
	INT8U v_flag;
	INT8U v_temp;
	INT8 v_sign;
	
	// Max Integer (Unsigned) = 4294967296
	// Max Integer (Signed)   = +/-2147483648
	v_idx = 0;
	p_char = p_string;
	v_flag = 1;
	v_result = 0;
	v_sign = 1; // assume positive number initially
	
	while ( v_flag == 1)			
	{
		// Check input string for valid chars
		// Accept +/- only on the beginning 
		switch (*p_char)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				p_char++;
				v_idx++;
				break;
				
			case '-':
				v_sign = -1;  // reverse sign when needed
			case '+': 
				// on positive sign do nothing, we already assumed that
				// Check proper notation, +/- sign allowed only in the beginning
				if (v_idx != 0)
				{
					v_flag = 0;
				}
				else
				{
					p_char++;
					v_idx++;					
				}
				break;
				
			case 0: // normal termination
			case ' ':			
				v_flag = 2;
				break;
							
			default:
				v_flag = 0;
		}
		
		// Too many chars stop further processing
		if ( v_idx > 11) v_flag = 0;
		
	}
	
	// if no valid chars processed
	if ( v_idx == 0) v_flag = 0;
	
	// if v_idx == 0 then return default value (0)
	if (v_flag != 0) 	
	{
		// valid data, process
		--p_char;
		v_maxidx = v_idx;
		for (v_idx=0; v_idx < v_maxidx; v_idx++)
		{
			if ( (*p_char != '+') && (*p_char != '-') )
			{
				v_temp = (INT8U) (*p_char - '0');
				v_result +=  v_temp * f_TypeConv_Exp(10, v_idx);
				p_char--;
			}
			else
			{
				// Should be the last char processed
				v_result *= v_sign;
				p_char--;
			}
		}
			
	}
		
	return v_result;
}


