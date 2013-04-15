/*
             LUFA Library
     Copyright (C) Dean Camera, 2012.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
 */

/*
  Copyright 2012  Mark Ding (mark.ding@hotmail.com)

  Permission to use, copy, modify, distribute, and sell this 
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in 
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting 
  documentation, and that the name of the author not be used in 
  advertising or publicity pertaining to distribution of the 
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
 */

#include "../../../../Common/Common.h"


#define  __INCLUDE_FROM_USB_DRIVER
#include "../USBMode.h"


#include "EndpointStream_SIM3U.h"

#if !defined(CONTROL_ONLY_DEVICE)
uint8_t Endpoint_Discard_Stream(uint16_t Length,
		uint16_t* const BytesProcessed)
{
	uint8_t  ErrorCode;
	uint16_t BytesInTransfer = 0;

	if ((ErrorCode = Endpoint_WaitUntilReady()))
		return ErrorCode;

	if (BytesProcessed != NULL)
		Length -= *BytesProcessed;

	while (Length)
	{
		if (!(Endpoint_IsReadWriteAllowed()))
		{
			Endpoint_ClearOUT();

			if (BytesProcessed != NULL)
			{
				*BytesProcessed += BytesInTransfer;
				return ENDPOINT_RWSTREAM_IncompleteTransfer;
			}

			if ((ErrorCode = Endpoint_WaitUntilReady()))
				return ErrorCode;
		}
		else
		{
			Endpoint_Discard_8();

			Length--;
			BytesInTransfer++;
		}
	}

	return ENDPOINT_RWSTREAM_NoError;
}

uint8_t Endpoint_Null_Stream(uint16_t Length,
		uint16_t* const BytesProcessed)
{
	uint8_t  ErrorCode;
	uint16_t BytesInTransfer = 0;

	if ((ErrorCode = Endpoint_WaitUntilReady()))
		return ErrorCode;

	if (BytesProcessed != NULL)
		Length -= *BytesProcessed;

	while (Length)
	{
		if (!(Endpoint_IsReadWriteAllowed()))
		{
			Endpoint_ClearIN();

			if (BytesProcessed != NULL)
			{
				*BytesProcessed += BytesInTransfer;
				return ENDPOINT_RWSTREAM_IncompleteTransfer;
			}

			if ((ErrorCode = Endpoint_WaitUntilReady()))
				return ErrorCode;
		}
		else
		{
			Endpoint_Write_8(0);

			Length--;
			BytesInTransfer++;
		}
	}

	return ENDPOINT_RWSTREAM_NoError;
}
//------------------------------------------------------------------------------
uint32_t USB0_EP0_read_fifo(uint8_t * dst,  uint32_t count)
{
	uint32_t * pTmp32;
	uint32_t result;
	count = MIN( count, SI32_USB_A_read_ep0_count(SI32_USB_0) );
	result=count;
	while( (((uint32_t) dst) & 0x3) && count)
	{
		*dst++=SI32_USB_A_read_ep0_fifo_u8(SI32_USB_0);
		count--;
	};

	pTmp32=(uint32_t*) dst;
	while(count>3)
	{
		*pTmp32++=SI32_USB_A_read_ep0_fifo_u32(SI32_USB_0);
		count-=4;
	}
	dst = (uint8_t*) pTmp32;
	while(count)
	{
		*dst++=(uint32_t) SI32_USB_A_read_ep0_fifo_u8(SI32_USB_0);
		count--;
	}
	return result;
}

//------------------------------------------------------------------------------
uint32_t USB0_EP0_write_fifo(uint8_t * src, uint32_t count)
{
	uint32_t * pTmp32;
	uint32_t result;
	result=count=MIN( count, ENDPOINT_CONTROLEP_DEFAULT_SIZE);

	// ensure src buffer is 32-bit aligned
	while( (((uint32_t) src) & 0x3) && count)
	{
		SI32_USB_A_write_ep0_fifo_u8(SI32_USB_0, *src);
		src++;
		count--;
	}

	// use 32-bit fifo writes, now that we know that the src buffer is aligned.
	pTmp32=(uint32_t*) src;
	while( count > 3 )
	{
		SI32_USB_A_write_ep0_fifo_u32(SI32_USB_0, *pTmp32);
		pTmp32++;
		count-=4;
	}
	src = (uint8_t*) pTmp32;

	// Write out any residue bytes 1 byte at a time
	while(count)
	{
		SI32_USB_A_write_ep0_fifo_u8(SI32_USB_0, *src);
		src++;
		count--;
	}
	return result;
}
//------------------------------------------------------------------------------
uint32_t USB0_EPn_read_fifo(SI32_USBEP_A_Type *ep,  uint8_t * dst,  uint32_t count)
{
	uint32_t * pTmp32;
	uint32_t result;
	count = MIN( count, SI32_USBEP_A_read_data_count(ep) );
	result=count;

	while ( (((uint32_t) dst) & 0x3) && count)
	{
		*dst++=SI32_USBEP_A_read_fifo_u8(ep);
		count--;
	};

	pTmp32=(uint32_t*) dst;
	while (count>3)
	{
		*pTmp32++=SI32_USBEP_A_read_fifo_u32(ep);
		count-=4;
	}
	dst = (uint8_t*) pTmp32;
	while (count)
	{
		*dst++=(uint32_t) SI32_USBEP_A_read_fifo_u8(ep);
		count--;
	}
	return result;
}


//------------------------------------------------------------------------------
uint32_t USB0_EPn_write_fifo(SI32_USBEP_A_Type *ep, uint8_t * src, uint32_t count)
{
	uint32_t * pTmp32;
	uint32_t result;
	result=count=MIN( count, (SI32_USBEP_A_get_in_max_packet_size(ep)<<3));

	// ensure src buffer is 32-bit aligned
	while ( (((uint32_t) src) & 0x3) && count)
	{
		SI32_USBEP_A_write_fifo_u8(ep, *src);
		src++;
		count--;
	}

	// use 32-bit fifo writes, now that we know that the src buffer is aligned.
	pTmp32=(uint32_t*) src;
	while ( count > 3 )
	{
		SI32_USBEP_A_write_fifo_u32(ep, *pTmp32);
		pTmp32++;
		count-=4;
	}
	src = (uint8_t*) pTmp32;

	// Write out any residue bytes 1 byte at a time
	while (count)
	{
		SI32_USBEP_A_write_fifo_u8(ep, *src);
		src++;
		count--;
	}
	return result;
}
uint8_t Endpoint_Write_Stream_LE(const void* const Buffer,
		uint16_t Length,
		uint16_t* const BytesProcessed)
{
	uint32_t count;
	uint8_t ErrorCode;
	uint8_t * buf = (uint8_t *)Buffer;

	if(usb_ep_selected == ENDPOINT_CONTROLEP)
	{
		do
		{
			if ((ErrorCode = Endpoint_WaitUntilReady()) != ENDPOINT_READYWAIT_NoError)
			  return ErrorCode;
			if(Length > ENDPOINT_CONTROLEP_DEFAULT_SIZE)
			{
				count = USB0_EP0_write_fifo(buf, ENDPOINT_CONTROLEP_DEFAULT_SIZE);
				buf += count;
				SI32_USB_A_set_in_packet_ready_ep0(SI32_USB_0);
			}
			else
			{
				count = USB0_EP0_write_fifo(buf, Length);
			}
			Length -= count;
		}while(Length);
	}
	else
	{
		SI32_USBEP_A_Type *ep = USB_EPn(usb_ep_selected);
		uint32_t ep_size = SI32_USBEP_A_get_in_max_packet_size(ep)<<3;
		do
		{
			if ((ErrorCode = Endpoint_WaitUntilReady()) != ENDPOINT_READYWAIT_NoError)
				return ErrorCode;
			if(Length > ep_size)
			{
				count = USB0_EPn_write_fifo(ep,buf, ep_size);
				buf += count;
				SI32_USBEP_A_set_in_packet_ready(ep);
			}
			else
			{
				count = USB0_EPn_write_fifo(ep,buf, Length);
			}
			Length -= count;
		}while(Length);
	}
	return ENDPOINT_RWSTREAM_NoError;

}

uint8_t Endpoint_Read_Stream_LE(void* const Buffer,
		uint16_t Length,
		uint16_t* const BytesProcessed)
{
	uint16_t i;  // TODO fine tune this read stream things
	for(i=0;i<Length;i++)
	{
		((uint8_t*)Buffer)[i] = Endpoint_Read_8();
	}
	return ENDPOINT_RWSTREAM_NoError;
}


uint8_t Endpoint_Write_Control_Stream_LE(const void* const Buffer,
		uint16_t Length)
{
	// TODO should we consider add endpoint 0 selected before ?
	Endpoint_Write_Stream_LE((uint8_t*)Buffer, MIN(Length, USB_ControlRequest.wLength), NULL);
	Endpoint_ClearIN();
	return ENDPOINT_RWCSTREAM_NoError;
}


uint8_t Endpoint_Read_Control_Stream_LE(void* const Buffer,
		uint16_t Length)
{
	while(!Endpoint_IsOUTReceived());
	Endpoint_Read_Stream_LE(Buffer,Length,NULL);
	//	Endpoint_ClearOUT();
	return ENDPOINT_RWCSTREAM_NoError;
}

#endif
