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
#include "circular_buffer.h"
#define  __INCLUDE_FROM_USB_DRIVER
#include "../USBInterrupt.h"
extern uint32_t usb_EPn_read_fifo(SI32_USBEP_A_Type *ep,circular_buffer_pools_t *cb_out);
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
//extern void EVENT_USB_recv_data(void);
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
		ep->EPCONTROL.U32 |= (SI32_USBEP_A_EPCONTROL_OSTSTLI_NOT_SET_U32
		        | SI32_USBEP_A_EPCONTROL_OORF_SET_U32
#ifdef ENDPOINT_SPLIT
		        | SI32_USBEP_A_EPCONTROL_SPLITEN_ENABLED_U32
#endif
				| SI32_USBEP_A_EPCONTROL_ISTSTLI_SET_U32
				| SI32_USBEP_A_EPCONTROL_IURF_SET_U32);
	}
}

void USB0_ep0_handler(void)
{
   uint32_t ControlReg = SI32_USB_A_read_ep0control(SI32_USB_0);

   uint32_t ep_num_backup = usb_ep_selected;
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
   Endpoint_SelectEndpoint(ep_num_backup);
}

extern void EVENT_USB_common_request(void);
//==============================================================================
//1st LEVEL  INTERRUPT HANDLERS
//==============================================================================
void USB0_IRQHandler(void)
{
    SI32_USBEP_A_Type * ep;
    circular_buffer_pools_t *cb_in, *cb_out;
    uint32_t usb_cmint_mask = SI32_USB_A_read_cmint(SI32_USB_0);
    uint32_t usb_ioint_mask = SI32_USB_A_read_ioint(SI32_USB_0);

    SI32_USB_A_write_cmint(SI32_USB_0, usb_cmint_mask);
    SI32_USB_A_write_ioint(SI32_USB_0, usb_ioint_mask);

    if (usb_ioint_mask & SI32_USB_A_IOINT_EP0I_MASK)
    {
        USB0_ep0_handler();
        return;
    }

    EVENT_USB_common_request();
#if 0
    // Handle Start of Frame Interrupt
    if (usb_cmint_mask & SI32_USB_A_CMINT_SOFI_MASK)//    Use the SOF interrupt to get OUT packets being naked from host
    {
        for(int i = 0; i < EPn_NUMBER; i++)
        {
            ep = USB_EPn(i + 1);
            cb_out = circular_buffer_pointer(i + 1);
            if(circular_buffer_write_ready(cb_out) == 0)
            {
                if(circular_buffer_remain_count(cb_out) >= circular_buffer_ep_size(cb_out))
                {
                    usb_EPn_read_fifo(ep, cb_out);
                    circular_buffer_put_write_ready(cb_out,1);
                }
            }
        }
    //    EVENT_USB_Device_StartOfFrame();
    }

    for(int i = 0; i < EPn_NUMBER; i++)
    {
        ep = USB_EPn(i + 1);
        cb_in = circular_buffer_pointer((i + 1)|0x80);
        usb_EPn_write_fifo(ep,cb_in,i);
        if(usb_ioint_mask & (SI32_USB_A_IOINT_OUT1I_MASK << i))
        {
            cb_out = circular_buffer_pointer(i + 1);
            if(circular_buffer_remain_count(cb_out) >= circular_buffer_ep_size(cb_out))
            {
                usb_EPn_read_fifo(ep, cb_out);
                circular_buffer_put_write_ready(cb_out,1);
            }
            else
            {
                circular_buffer_put_write_ready(cb_out,0);
            }
        }
    }
#endif
    // Handle Resume Interrupt
    if (usb_cmint_mask & SI32_USB_A_CMINT_RESI_MASK)
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
    if (usb_cmint_mask & SI32_USB_A_CMINT_RSTI_MASK)
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
    if (usb_cmint_mask & SI32_USB_A_CMINT_SUSI_MASK)
    {
        SI32_USB_A_disable_suspend_interrupt(SI32_USB_0);
        SI32_USB_A_enable_resume_interrupt(SI32_USB_0);
        USB_CLK_Freeze();
        USB_DeviceState = DEVICE_STATE_Suspended;
        //	  EVENT_USB_Device_Suspend();
    }
}


