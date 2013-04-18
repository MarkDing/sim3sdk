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
 *  Header file for Descriptors.c.
 */

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include <LUFA/Drivers/USB/USB.h>

	/* Macros: */
        #define DUAL_VCP_ENABLE 1
        #define TRI_VCP_ENABLE  0

		/** Endpoint address of the CDC device-to-host notification IN endpoint. */
		#define CDC_NOTIFICATION_EPADDR        (ENDPOINT_DIR_IN  | 1)

        /** Endpoint address of the CDC1 device-to-host data IN endpoint. */
		#define CDC1_TX_EPADDR                  (ENDPOINT_DIR_IN  | 2)

		/** Endpoint address of the CDC1 host-to-device data OUT endpoint. */
		#define CDC1_RX_EPADDR                  (ENDPOINT_DIR_OUT | 2)

#if (DUAL_VCP_ENABLE || TRI_VCP_ENABLE)
        /** Endpoint address of the CDC2 device-to-host data IN endpoint. */
        #define CDC2_TX_EPADDR                  (ENDPOINT_DIR_IN  | 3)

        /** Endpoint address of the CDC2 host-to-device data OUT endpoint. */
        #define CDC2_RX_EPADDR                  (ENDPOINT_DIR_OUT | 3)
#endif

#if (TRI_VCP_ENABLE)
        /** Endpoint address of the CDC3 device-to-host data IN endpoint. */
        #define CDC3_TX_EPADDR                  (ENDPOINT_DIR_IN  | 4)

        /** Endpoint address of the CDC3 host-to-device data OUT endpoint. */
        #define CDC3_RX_EPADDR                  (ENDPOINT_DIR_OUT | 4)
#endif
		/** Size in bytes of the CDC device-to-host notification IN endpoint. */
		#define CDC_NOTIFICATION_EPSIZE        16

		/** Size in bytes of the CDC data IN and OUT endpoints. */
		#define CDC_TXRX_EPSIZE                64

	/* Type Defines: */
		/** Type define for the device configuration descriptor structure. This must be defined in the
		 *  application code, as the configuration descriptor contains several sub-descriptors which
		 *  vary between devices, and which describe the device's usage to the host.
		 */
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t    Config;

			// CDC1 Control Interface
			USB_Descriptor_Interface_Association_t   CDC1_IAD;
			USB_Descriptor_Interface_t               CDC1_CCI_Interface;
			USB_CDC_Descriptor_FunctionalHeader_t    CDC1_Functional_Header;
			USB_CDC_Descriptor_FunctionalACM_t       CDC1_Functional_ACM;
			USB_CDC_Descriptor_FunctionalUnion_t     CDC1_Functional_Union;
			USB_Descriptor_Endpoint_t                CDC1_NotificationEndpoint;

			// CDC1 Data Interface
			USB_Descriptor_Interface_t               CDC1_DCI_Interface;
			USB_Descriptor_Endpoint_t                CDC1_DataOutEndpoint;
			USB_Descriptor_Endpoint_t                CDC1_DataInEndpoint;

#if (DUAL_VCP_ENABLE || TRI_VCP_ENABLE)
			// CDC2 Control Interface
			USB_Descriptor_Interface_Association_t   CDC2_IAD;
            USB_Descriptor_Interface_t               CDC2_CCI_Interface;
            USB_CDC_Descriptor_FunctionalHeader_t    CDC2_Functional_Header;
            USB_CDC_Descriptor_FunctionalACM_t       CDC2_Functional_ACM;
            USB_CDC_Descriptor_FunctionalUnion_t     CDC2_Functional_Union;
            USB_Descriptor_Endpoint_t                CDC2_NotificationEndpoint;

            // CDC2 Data Interface
            USB_Descriptor_Interface_t               CDC2_DCI_Interface;
            USB_Descriptor_Endpoint_t                CDC2_DataOutEndpoint;
            USB_Descriptor_Endpoint_t                CDC2_DataInEndpoint;
#endif

#if (TRI_VCP_ENABLE)
            // CDC3 Control Interface
            USB_Descriptor_Interface_Association_t   CDC3_IAD;
            USB_Descriptor_Interface_t               CDC3_CCI_Interface;
            USB_CDC_Descriptor_FunctionalHeader_t    CDC3_Functional_Header;
            USB_CDC_Descriptor_FunctionalACM_t       CDC3_Functional_ACM;
            USB_CDC_Descriptor_FunctionalUnion_t     CDC3_Functional_Union;
            USB_Descriptor_Endpoint_t                CDC3_NotificationEndpoint;

            // CDC3 Data Interface
            USB_Descriptor_Interface_t               CDC3_DCI_Interface;
            USB_Descriptor_Endpoint_t                CDC3_DataOutEndpoint;
            USB_Descriptor_Endpoint_t                CDC3_DataInEndpoint;
#endif
		} USB_Descriptor_Configuration_t;

	/* Function Prototypes: */
		uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
		                                    const uint8_t wIndex,
		                                    const void** const DescriptorAddress)
		                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif

