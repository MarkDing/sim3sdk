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
#define  __INCLUDE_FROM_USB_CONTROLLER_C
#include "../USBController.h"

#if !defined(USE_STATIC_OPTIONS)
volatile uint8_t USB_Options;
#endif

void USB_Init(const uint8_t Options)
{
	#if !defined(USE_STATIC_OPTIONS)
	USB_Options = Options;
	#endif

	USB_IsInitialized = true;

	SI32_USB_A_enable_usb_oscillator(SI32_USB_0);

	// Perform asynchronous reset of the USB module
	SI32_USB_A_reset_module (SI32_USB_0);
	USB_CLK_Unfreeze();

	// Enable Endpoint 0 interrupts
	SI32_USB_A_write_cmint (SI32_USB_0, 0x00000000);
	SI32_USB_A_write_ioint (SI32_USB_0, 0x00000000);
	SI32_USB_A_enable_ep0_interrupt (SI32_USB_0);

	USB_DeviceState                 = DEVICE_STATE_Unattached;
	USB_Device_ConfigurationNumber  = 0;

	#if !defined(NO_DEVICE_REMOTE_WAKEUP)
	USB_Device_RemoteWakeupEnabled  = false;
	#endif

	#if !defined(NO_DEVICE_SELF_POWER)
	USB_Device_CurrentlySelfPowered = false;
	#endif

	if (USB_Options & USB_DEVICE_OPT_LOWSPEED)
	{
		USB_Device_SetLowSpeed();
	}
	else
	{
		USB_Device_SetFullSpeed();
	}

	Endpoint_ConfigureEndpoint(ENDPOINT_CONTROLEP, EP_TYPE_CONTROL,
							   USB_Device_ControlEndpointSize, 1);
	// Enable Reset, Resume, Suspend interrupts
	SI32_USB_A_enable_suspend_interrupt (SI32_USB_0);
//	SI32_USB_A_enable_resume_interrupt (SI32_USB_0);
	SI32_USB_A_enable_reset_interrupt (SI32_USB_0);
//	SI32_USB_A_enable_start_of_frame_interrupt (SI32_USB_0);
	// Enable USB interrupts
	NVIC_EnableIRQ(USB0_IRQn);

	USB_DeviceState = DEVICE_STATE_Powered;
	// Uninhibit the module once all initialization is complete
	SI32_USB_A_enable_module(SI32_USB_0);
    SI32_USB_A_enable_internal_pull_up(SI32_USB_0);
}

void USB_Disable(void)
{
	USB_INT_DisableAllInterrupts();
	USB_INT_ClearAllInterrupts();

	USB_Controller_Disable();

	USB_IsInitialized = false;
}



