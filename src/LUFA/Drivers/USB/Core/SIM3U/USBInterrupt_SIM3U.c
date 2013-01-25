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
#include "../USBInterrupt.h"

void USB_INT_DisableAllInterrupts(void)
{
	SI32_USB_A_write_cmintepe(SI32_USB_0, 0x0);
	SI32_USB_A_write_iointe(SI32_USB_0, 0x0);
}

void USB_INT_ClearAllInterrupts(void)
{
	SI32_USB_A_write_cmint(SI32_USB_0, 0xF);
	SI32_USB_A_write_ioint(SI32_USB_0, 0x001E001F);
}
extern void EVENT_USB_recv_data(void);
//------------------------------------------------------------------------------
void USB0_epn_handler(void)
{
	uint32_t control_reg;
	SI32_USBEP_A_Type * ep = USB_EPn(usb_ep_selected);

	control_reg = SI32_USBEP_A_read_epcontrol(ep);

	// Hardware sets the OPRDYI bit to 1 once out packet ready.
#if 0
 	if(control_reg & SI32_USBEP_A_EPCONTROL_OPRDYI_MASK)
	{
		EVENT_USB_recv_data();
	}
#endif
	if((control_reg & SI32_USBEP_A_EPCONTROL_OSTSTLI_MASK) ||
		(control_reg & SI32_USBEP_A_EPCONTROL_ISTSTLI_MASK))
	{
		ep->EPCONTROL.U32 |= (SI32_USBEP_A_EPCONTROL_OSTSTLI_MASK | SI32_USBEP_A_EPCONTROL_OORF_SET_U32
				|SI32_USBEP_A_EPCONTROL_ISTSTLI_SET_U32|SI32_USBEP_A_EPCONTROL_IURF_SET_U32);
	}
}

void USB0_ep0_handler(void)
{
   uint32_t ControlReg = SI32_USB_A_read_ep0control(SI32_USB_0);

   Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
   if(ControlReg & SI32_USB_A_EP0CONTROL_STSTLI_MASK)
   {
      SI32_USB_A_clear_stall_sent_ep0(SI32_USB_0);
   }
   if(ControlReg & SI32_USB_A_EP0CONTROL_SUENDI_MASK)
   {
      SI32_USB_A_clear_setup_end_early_ep0(SI32_USB_0);
   }
   if(ControlReg & SI32_USB_A_EP0CONTROL_OPRDYI_MASK)
   {
	   USB_Device_ProcessControlRequest();
   }
}

//==============================================================================
//1st LEVEL  INTERRUPT HANDLERS
//==============================================================================
void USB0_IRQHandler(void)
{
  uint32_t usbCommonInterruptMask = SI32_USB_A_read_cmint(SI32_USB_0);
  uint32_t usbEpInterruptMask = SI32_USB_A_read_ioint(SI32_USB_0);

  SI32_USB_A_write_cmint(SI32_USB_0, usbCommonInterruptMask);
  SI32_USB_A_write_ioint(SI32_USB_0, usbEpInterruptMask);

  if (usbEpInterruptMask & SI32_USB_A_IOINT_EP0I_MASK)
  {
	  USB0_ep0_handler();
	  return;
  }

  if (usbEpInterruptMask & (SI32_USB_A_IOINT_IN1I_MASK | SI32_USB_A_IOINT_OUT1I_MASK))
  {
	  Endpoint_SelectEndpoint(1);
	  USB0_epn_handler();
  }
  if (usbEpInterruptMask & (SI32_USB_A_IOINT_IN2I_MASK | SI32_USB_A_IOINT_OUT2I_MASK))
  {
	  Endpoint_SelectEndpoint(2);
	  USB0_epn_handler();
  }
  if (usbEpInterruptMask & (SI32_USB_A_IOINT_IN3I_MASK | SI32_USB_A_IOINT_OUT3I_MASK))
  {
	  Endpoint_SelectEndpoint(3);
	  USB0_epn_handler();
  }
  if (usbEpInterruptMask & (SI32_USB_A_IOINT_IN4I_MASK | SI32_USB_A_IOINT_OUT4I_MASK))
  {
	  Endpoint_SelectEndpoint(4);
	  USB0_epn_handler();
  }

  // Handle Start of Frame Interrupt
  if (usbCommonInterruptMask & SI32_USB_A_CMINT_SOFI_MASK)
  {
//	  EVENT_USB_Device_StartOfFrame();
  }

  // Handle Resume Interrupt
  if (usbCommonInterruptMask & SI32_USB_A_CMINT_RESI_MASK)
  {
		USB_CLK_Unfreeze();
		SI32_USB_A_disable_resume_interrupt(SI32_USB_0);
		SI32_USB_A_enable_suspend_interrupt(SI32_USB_0);

		if (USB_Device_ConfigurationNumber)
		  USB_DeviceState = DEVICE_STATE_Configured;
		else
		  USB_DeviceState = (USB_Device_IsAddressSet()) ? DEVICE_STATE_Configured : DEVICE_STATE_Powered;
		EVENT_USB_Device_WakeUp();
  }

  // Handle Reset Interrupt
  if (usbCommonInterruptMask & SI32_USB_A_CMINT_RSTI_MASK)
  {
	  SI32_USB_A_enable_ep0(SI32_USB_0);
	  SI32_USB_A_disable_ep1(SI32_USB_0);
	  SI32_USB_A_disable_ep2(SI32_USB_0);
	  SI32_USB_A_disable_ep3(SI32_USB_0);
	  SI32_USB_A_disable_ep4(SI32_USB_0);

	  USB_DeviceState                = DEVICE_STATE_Default;
	  USB_Device_ConfigurationNumber = 0;

	  Endpoint_ConfigureEndpoint(ENDPOINT_CONTROLEP, EP_TYPE_CONTROL,
		                           USB_Device_ControlEndpointSize, 1);
//	  EVENT_USB_Device_Reset();
  }

  // Handle Suspend interrupt
  if (usbCommonInterruptMask & SI32_USB_A_CMINT_SUSI_MASK)
  {
	  SI32_USB_A_disable_suspend_interrupt(SI32_USB_0);
	  SI32_USB_A_enable_resume_interrupt(SI32_USB_0);
	  USB_CLK_Freeze();
	  USB_DeviceState = DEVICE_STATE_Suspended;
//	  EVENT_USB_Device_Suspend();
  }
}


