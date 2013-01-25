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
#include "gUART0.h"
extern void EVENT_USB_recv_data(void);
extern void EVENT_USB_send_data(void);
/** Circular buffer to hold data from the host before it is sent to the device via the serial port. */
static RingBuffer_t USBtoUSART_Buffer;

/** Underlying data buffer for \ref USBtoUSART_Buffer, where the stored bytes are located. */
static uint8_t      USBtoUSART_Buffer_Data[256];

/** Circular buffer to hold data from the serial port before it is sent to the host. */
static RingBuffer_t USARTtoUSB_Buffer;

/** Underlying data buffer for \ref USARTtoUSB_Buffer, where the stored bytes are located. */
static uint8_t      USARTtoUSB_Buffer_Data[256];

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
{
	.Config =
	{
		.ControlInterfaceNumber   = 0,
		.DataINEndpoint           =
		{
			.Address          = CDC_TX_EPADDR,
			.Size             = CDC_TXRX_EPSIZE,
			.Banks            = 1,
		},
		.DataOUTEndpoint =
		{
			.Address          = CDC_RX_EPADDR,
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

void uart_rev_handler(void)
{
	uint8_t count = SI32_UART_A_read_rx_fifo_count(SI32_UART_0);
	uint8_t ReceivedByte;

	while(count--)
	{
		ReceivedByte = uart_get_byte();
		if (USB_DeviceState == DEVICE_STATE_Configured)
		{
			RingBuffer_Insert(&USARTtoUSB_Buffer, ReceivedByte);
		}
	}
}

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int vcp_main(void)
{
    USB_Init(USB_DEVICE_OPT_FULLSPEED);
	RingBuffer_InitBuffer(&USBtoUSART_Buffer, USBtoUSART_Buffer_Data, sizeof(USBtoUSART_Buffer_Data));
	RingBuffer_InitBuffer(&USARTtoUSB_Buffer, USARTtoUSB_Buffer_Data, sizeof(USARTtoUSB_Buffer_Data));

	while(1)
	{
		EVENT_USB_recv_data();
		EVENT_USB_send_data();
	}
}

void EVENT_USB_send_data(void)
{
	uint16_t BufferCount = RingBuffer_GetCount(&USARTtoUSB_Buffer);
	if(BufferCount > 0)
	{
		/* Read bytes from the USART receive buffer into the USB IN endpoint */
		while (BufferCount--)
		{
			/* Try to send the next byte of data to the host, abort if there is an error without dequeuing */
			if (CDC_Device_SendByte(&VirtualSerial_CDC_Interface,
									RingBuffer_Peek(&USARTtoUSB_Buffer)) != ENDPOINT_READYWAIT_NoError)
			{
				break;
			}
			/* Dequeue the already sent byte from the buffer now we have confirmed that no transmission error occurred */
			RingBuffer_Remove(&USARTtoUSB_Buffer);
		}
		Endpoint_ClearIN();
	}
}

void EVENT_USB_recv_data(void)
{
	uint16_t count, ReceivedByte;
	count = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
	while(count--)
	{
		// Only try to read in bytes from the CDC interface if the transmit buffer is not full
		if (!(RingBuffer_IsFull(&USBtoUSART_Buffer)))
		{
			ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
			// Read bytes from the USB OUT endpoint into the USART transmit buffer
			if (!(ReceivedByte < 0))
			{
			  RingBuffer_Insert(&USBtoUSART_Buffer, ReceivedByte);
			  uart_send_byte(RingBuffer_Remove(&USBtoUSART_Buffer));
			}
		}
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

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
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

