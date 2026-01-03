/****************************************************************************
Project: L2C V2.00 Kernel
Title: Type Definitions
File: fifo.c
Last updated: 30 Oct 2009
Version: 0.01
****************************************************************************/

typedef struct 
{
	INT16U v_WrIdx;
	INT16U v_RdIdx;
	INT16U v_Length;
	INT16U v_MaxLen;
	PINT8U p_Buffer;
	PINT8U p_BufferHi;
} type_fifo;

#define c_FIFOEMPTY 2
#define c_FIFOFULL  1
#define c_FIFOOK   0


void f_Fifo_Init(type_fifo *p_fifo, PINT8U p_Buffer, PINT8U p_BufferHi, INT16U v_MaxLen);
INT8U f_Fifo_GetBytes(type_fifo *p_fifo, PINT8U p_data, PINT8U p_dataHi);
INT8U f_Fifo_PutBytes(type_fifo *p_fifo, INT8U v_data, INT8U v_dataHi);
INT16U f_Fifo_GetLength(type_fifo *p_fifo);
