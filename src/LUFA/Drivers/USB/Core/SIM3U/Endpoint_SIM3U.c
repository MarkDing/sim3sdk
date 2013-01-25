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

#if defined(USB_CAN_BE_DEVICE)

#include "../Endpoint.h"

#if !defined(FIXED_CONTROL_ENDPOINT_SIZE)
uint8_t USB_Device_ControlEndpointSize = ENDPOINT_CONTROLEP_DEFAULT_SIZE;
#endif

volatile uint32_t usb_ep_selected = ENDPOINT_CONTROLEP;

bool Endpoint_ConfigureEndpoint(const uint8_t Address,
                                             const uint8_t Type,
                                             const uint16_t Size,
                                             const uint8_t Banks)
{
	uint8_t Number = (Address & ENDPOINT_EPNUM_MASK)% ENDPOINT_TOTAL_ENDPOINTS;
	uint8_t dir_sel =  (Address & 0x80)?1:0;
	uint32_t ep_config = 0;

	if(Number != ENDPOINT_CONTROLEP)
	{
		ep_config = 0x450024; // bit 2,5,16,18,22 must be writen

		if(Type == EP_TYPE_ISOCHRONOUS)
		{   // ISO transfer need set "1"
			ep_config |= (SI32_USBEP_A_EPCONTROL_IISOEN_ISO_U32);
		}
		if(dir_sel)
		{   // IN endpoint
			ep_config |= (SI32_USBEP_A_EPCONTROL_DIRSEL_IN_U32|SI32_USBEP_A_EPCONTROL_ICLRDT_RESET_U32);
			SI32_USBEP_A_set_in_max_packet_size(USB_EPn(Number),Size>>3);
		}
		else
		{   // OUT endpoint
			ep_config |= (SI32_USBEP_A_EPCONTROL_OCLRDT_RESET_U32);
			SI32_USBEP_A_set_out_max_packet_size(USB_EPn(Number),Size>>3);
		}
		USB_EPn(Number)->EPCONTROL.U32 = ep_config;
	}
	Endpoint_SelectEndpoint(Number);
	Endpoint_EnableEndpoint();
	return true;
}



bool Endpoint_ConfigureEndpointTable(const USB_Endpoint_Table_t* const Table,
                                     const uint8_t Entries)
{
	for (uint8_t i = 0; i < Entries; i++)
	{
		if (!(Table[i].Address))
		  continue;

		if (!(Endpoint_ConfigureEndpoint(Table[i].Address, Table[i].Type, Table[i].Size, Table[i].Banks)))
		{
			return false;
		}
	}

	return true;
}


void Endpoint_ClearEndpoints(void)
{
	for (uint8_t EPNum = 0; EPNum < ENDPOINT_TOTAL_ENDPOINTS; EPNum++)
	{
		Endpoint_SelectEndpoint(EPNum);
		Endpoint_DisableEndpoint();
	}
}

void Endpoint_ClearStatusStage(void)
{
#if 0
	if (USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST)
	{
		Endpoint_ClearOUT();
	}
	else
	{
		Endpoint_ClearIN();
	}
#endif
}

#if !defined(CONTROL_ONLY_DEVICE)
uint8_t Endpoint_WaitUntilReady(void)
{
	#if (USB_STREAM_TIMEOUT_MS < 0xFF)
	uint8_t  TimeoutMSRem = USB_STREAM_TIMEOUT_MS;
	#else
	uint16_t TimeoutMSRem = USB_STREAM_TIMEOUT_MS;
	#endif

	uint16_t PreviousFrameNumber = USB_Device_GetFrameNumber();

	for (;;)
	{
		if (Endpoint_GetEndpointDirection() == ENDPOINT_DIR_IN)
		{
			if (Endpoint_IsINReady())
			  return ENDPOINT_READYWAIT_NoError;
		}
		else
		{
			if (Endpoint_IsOUTReceived())
			  return ENDPOINT_READYWAIT_NoError;
		}

		uint8_t USB_DeviceState_LCL = USB_DeviceState;

		if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
		  return ENDPOINT_READYWAIT_DeviceDisconnected;
		else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
		  return ENDPOINT_READYWAIT_BusSuspended;
		else if (Endpoint_IsStalled())
		  return ENDPOINT_READYWAIT_EndpointStalled;

		uint16_t CurrentFrameNumber = USB_Device_GetFrameNumber();

		if (CurrentFrameNumber != PreviousFrameNumber)
		{
			PreviousFrameNumber = CurrentFrameNumber;

			if (!(TimeoutMSRem--))
			  return ENDPOINT_READYWAIT_Timeout;
		}
	}
}
#endif

#endif

