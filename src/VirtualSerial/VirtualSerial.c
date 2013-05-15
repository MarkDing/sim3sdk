/*
             LUFA Library
     Copyright (C) Dean Camera, 2012.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2012  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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

/** \file
 *
 *  Main source file for the VirtualSerial demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "VirtualSerial.h"
#include "circular_buffer.h"
#include "gUART0.h"
#include <SI32_TIMER_A_Type.h>
#include "gTimer.h"
void VCOM_bridge(void);
void VCOM_echo(void);

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC1_Interface =
{
	.Config =
	{
		.ControlInterfaceNumber   = 0,
		.DataINEndpoint           =
		{
			.Address          = CDC1_TX_EPADDR,
			.Size             = CDC_TXRX_EPSIZE,
			.Banks            = 1,
		},
		.DataOUTEndpoint =
		{
			.Address          = CDC1_RX_EPADDR,
			.Size             = CDC_TXRX_EPSIZE,
			.Banks            = 1,
		},
		.NotificationEndpoint =
		{
			.Address          = CDC_NOTIFICATION_EPADDR,
			.Size             = CDC_NOTIFICATION_EPSIZE,
			.Banks            = 1,
		},
	},
};


#if (DUAL_VCP_ENABLE || TRI_VCP_ENABLE)
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC2_Interface =
{
	.Config =
	{
		.ControlInterfaceNumber   = 2,
		.DataINEndpoint           =
		{
			.Address          = CDC2_TX_EPADDR,
			.Size             = CDC_TXRX_EPSIZE,
			.Banks            = 1,
		},
		.DataOUTEndpoint =
		{
			.Address          = CDC2_RX_EPADDR,
			.Size             = CDC_TXRX_EPSIZE,
			.Banks            = 1,
		},
		.NotificationEndpoint =
		{
			.Address          = CDC_NOTIFICATION_EPADDR,
			.Size             = CDC_NOTIFICATION_EPSIZE,
			.Banks            = 1,
		},
	},
};
#endif

#if (TRI_VCP_ENABLE)
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC3_Interface =
{
    .Config =
    {
        .ControlInterfaceNumber   = 4,
        .DataINEndpoint           =
        {
            .Address          = CDC3_TX_EPADDR,
            .Size             = CDC_TXRX_EPSIZE,
            .Banks            = 1,
        },
        .DataOUTEndpoint =
        {
            .Address          = CDC3_RX_EPADDR,
            .Size             = CDC_TXRX_EPSIZE,
            .Banks            = 1,
        },
        .NotificationEndpoint =
        {
            .Address          = CDC_NOTIFICATION_EPADDR,
            .Size             = CDC_NOTIFICATION_EPSIZE,
            .Banks            = 1,
        },
    },
};
#endif
/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
void app_main(void)
{
    USB_Init(USB_DEVICE_OPT_FULLSPEED);

	while(1)
	{
		VCOM_echo();
///		VCOM_bridge();
	}
}


void VCOM_echo(void)
{
    uint8_t temp;
    circular_buffer_pools_t * cb_out = circular_buffer_pointer(0x3);
    circular_buffer_pools_t * cb_in = circular_buffer_pointer(0x82);

    if((!circular_buffer_is_empty(cb_out)) && (!circular_buffer_is_full(cb_in)))
    {
        circular_buffer_pop(cb_out,&temp);
        circular_buffer_push(cb_in,&temp);
        //  Set Next Timer Reload to finish transfer after there is room in FIFO
        start_timer(1);
    }

}


void VCOM_bridge(void)
{
	uint32_t count;
    circular_buffer_pools_t * cb;

    cb = circular_buffer_pointer(0x2); // EP2 OUT
	if(circular_buffer_count(cb))
	{
	    (SI32_UART_0->CONTROL_SET = SI32_UART_A_CONTROL_TCPTI_MASK);
	}
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC1_Interface);
#if (DUAL_VCP_ENABLE || TRI_VCP_ENABLE)
	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC2_Interface);
#endif
#if (TRI_VCP_ENABLE)
    ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC3_Interface);
#endif
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC1_Interface);
#if (DUAL_VCP_ENABLE || TRI_VCP_ENABLE)
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC2_Interface);
#endif
#if (TRI_VCP_ENABLE)
    CDC_Device_ProcessControlRequest(&VirtualSerial_CDC3_Interface);
#endif
}


/** Event handler for the CDC Class driver Line Encoding Changed event.
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
	uart_configuration(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS,
			CDCInterfaceInfo->State.LineEncoding.CharFormat,
			CDCInterfaceInfo->State.LineEncoding.ParityType,
			CDCInterfaceInfo->State.LineEncoding.DataBits);
}
void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{

}

void EVENT_CDC_Device_BreakSent(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo,
                                const uint8_t Duration)
{

}

void EVENT_USB_Device_WakeUp(void)
{

}

