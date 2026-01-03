/****************************************************************************
Project: L2C V2.00 Kernel
Title: FIFO Generic
File: fifo.c
Last updated: 30 Oct 2009
Version: 0.01
****************************************************************************/

/* ***************************************************************************
*  PROJECT INCLUDE FILES
*/
#include "StdTypes.h"
#include "Fifo.h" 

/*
	INT8U v_WrIdx;
	INT8U v_RdIdx;
	INT8U v_Length;
	INT8U v_MaxLen;
	PINT8U p_Buffer;
*/

void f_Fifo_Init(type_fifo *p_fifo, PINT8U p_Buffer, PINT8U p_BufferHi, INT16U v_MaxLen)
{
	INT16U v_idx;
	
	/* Setup FIFO */
	p_fifo->v_MaxLen = v_MaxLen;
	p_fifo->p_Buffer = p_Buffer;
	p_fifo->p_BufferHi = p_BufferHi;
	p_fifo->v_Length = 0;
	
	/* Clear FIFO Contents */
	for(v_idx=0;v_idx<v_MaxLen; v_idx++)
	{
		*(p_fifo->p_Buffer+v_idx) = 0;
		if (p_fifo->p_BufferHi != NULL) *(p_fifo->p_BufferHi+v_idx) = 0;
	}
	
	
	/* Initialize Rd/Wr Pointers */
	p_fifo->v_RdIdx = 0;
	p_fifo->v_WrIdx = 0;

}


INT8U f_Fifo_GetBytes(type_fifo *p_fifo, PINT8U p_data, PINT8U p_dataHi)
{
	INT8U v_return;
	INT16U v_addr;
	
	v_return = c_FIFOEMPTY;
	
	if (p_fifo->v_RdIdx != p_fifo->v_WrIdx)
	{
		v_addr = MODMASK(p_fifo->v_RdIdx, p_fifo->v_MaxLen);
		*p_data = *(p_fifo->p_Buffer + v_addr);		
		
		if (p_fifo->p_BufferHi != NULL)
		{
			if (p_dataHi != NULL) *p_dataHi = *(p_fifo->p_BufferHi + v_addr);
		}
		
		p_fifo->v_RdIdx = MODINCR(p_fifo->v_RdIdx, p_fifo->v_MaxLen);
		
		/* Need to be Atomic Operation */
		p_fifo->v_Length -= 1;
		v_return = c_FIFOOK;
	}
	
	return v_return;

}


INT8U f_Fifo_PutBytes(type_fifo *p_fifo, INT8U v_data, INT8U v_dataHi)
{
	INT8U v_return;
	INT16 v_diff;
	INT16U v_addr;
	
	v_return = c_FIFOFULL;
	v_diff = p_fifo->v_WrIdx - p_fifo->v_RdIdx;
	
	if (v_diff < 0) v_diff += p_fifo->v_MaxLen;
	
	if (v_diff < p_fifo->v_MaxLen)
	{
		v_addr = MODMASK(p_fifo->v_WrIdx, p_fifo->v_MaxLen);
		*(p_fifo->p_Buffer + v_addr) = v_data;	
		if (p_fifo->p_BufferHi != NULL) *(p_fifo->p_BufferHi + v_addr) = v_dataHi;	
		p_fifo->v_WrIdx = MODINCR(p_fifo->v_WrIdx, p_fifo->v_MaxLen);
		
		/* Need to be Atomic Operation */
		p_fifo->v_Length += 1;
		v_return = c_FIFOOK;
	}
	else
	{
		v_return = c_FIFOFULL;
	}
	
	return v_return;

}

INT16U f_Fifo_GetLength(type_fifo *p_fifo)
{
	INT16 v_diff;
	
	v_diff = p_fifo->v_WrIdx - p_fifo->v_RdIdx;
	
	if (v_diff < 0) v_diff += p_fifo->v_MaxLen;
	
	return v_diff;
}
