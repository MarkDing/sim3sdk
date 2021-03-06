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
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.
 */

#include "Descriptors.h"


/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

    .USBSpecification       = VERSION_BCD(02.00),
    .Class                  = USB_CSCP_IADDeviceClass,
    .SubClass               = USB_CSCP_IADDeviceSubclass,
    .Protocol               = USB_CSCP_IADDeviceProtocol,

	.Endpoint0Size          = ENDPOINT_CONTROLEP_DEFAULT_SIZE,

	.VendorID               = 0x10C4,
#if (TRI_VCP_ENABLE)
    .ProductID              = 0xA013,
#elif (DUAL_VCP_ENABLE)
    .ProductID              = 0xA012,
#else
	.ProductID              = 0xA011,
#endif
	.ReleaseNumber          = VERSION_BCD(00.01),

	.ManufacturerStrIndex   = 0x01,
	.ProductStrIndex        = 0x02,
	.SerialNumStrIndex      = USE_INTERNAL_SERIAL,

	.NumberOfConfigurations = 1
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
	.Config =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

			.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),

#if (TRI_VCP_ENABLE)
            .TotalInterfaces        = 6,
#elif (DUAL_VCP_ENABLE)
            .TotalInterfaces        = 4,
#else
            .TotalInterfaces        = 2,
#endif
			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,

			.ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
		},

    .CDC1_IAD =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_Association_t), .Type = DTYPE_InterfaceAssociation},

            .FirstInterfaceIndex    = 0,
            .TotalInterfaces        = 2,

            .Class                  = CDC_CSCP_CDCClass,
            .SubClass               = CDC_CSCP_ACMSubclass,
            .Protocol               = CDC_CSCP_ATCommandProtocol,

            .IADStrIndex            = NO_DESCRIPTOR
        },

	.CDC1_CCI_Interface =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber        = 0,
			.AlternateSetting       = 0,

			.TotalEndpoints         = 1,

			.Class                  = CDC_CSCP_CDCClass,
			.SubClass               = CDC_CSCP_ACMSubclass,
			.Protocol               = CDC_CSCP_ATCommandProtocol,

			.InterfaceStrIndex      = NO_DESCRIPTOR
		},

	.CDC1_Functional_Header =
		{
			.Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalHeader_t), .Type = DTYPE_CSInterface},
			.Subtype                = CDC_DSUBTYPE_CSInterface_Header,

			.CDCSpecification       = VERSION_BCD(01.10),
		},

	.CDC1_Functional_ACM =
		{
			.Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalACM_t), .Type = DTYPE_CSInterface},
			.Subtype                = CDC_DSUBTYPE_CSInterface_ACM,

			.Capabilities           = 0x06,
		},

	.CDC1_Functional_Union =
		{
			.Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalUnion_t), .Type = DTYPE_CSInterface},
			.Subtype                = CDC_DSUBTYPE_CSInterface_Union,

			.MasterInterfaceNumber  = 0,
			.SlaveInterfaceNumber   = 1,
		},

	.CDC1_NotificationEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = CDC_NOTIFICATION_EPADDR,
			.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = CDC_NOTIFICATION_EPSIZE,
			.PollingIntervalMS      = 0xFF
		},

	.CDC1_DCI_Interface =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber        = 1,
			.AlternateSetting       = 0,

			.TotalEndpoints         = 2,

			.Class                  = CDC_CSCP_CDCDataClass,
			.SubClass               = CDC_CSCP_NoDataSubclass,
			.Protocol               = CDC_CSCP_NoDataProtocol,

			.InterfaceStrIndex      = NO_DESCRIPTOR
		},

	.CDC1_DataOutEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = CDC1_RX_EPADDR,
			.Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = CDC_TXRX_EPSIZE,
			.PollingIntervalMS      = 0x05
		},

	.CDC1_DataInEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = CDC1_TX_EPADDR,
			.Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = CDC_TXRX_EPSIZE,
			.PollingIntervalMS      = 0x05
		},
#if (DUAL_VCP_ENABLE || TRI_VCP_ENABLE)
    .CDC2_IAD =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_Association_t), .Type = DTYPE_InterfaceAssociation},

            .FirstInterfaceIndex    = 2,
            .TotalInterfaces        = 2,

            .Class                  = CDC_CSCP_CDCClass,
            .SubClass               = CDC_CSCP_ACMSubclass,
            .Protocol               = CDC_CSCP_ATCommandProtocol,

            .IADStrIndex            = NO_DESCRIPTOR
        },

    .CDC2_CCI_Interface =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

            .InterfaceNumber        = 2,
            .AlternateSetting       = 0,

            .TotalEndpoints         = 1,

            .Class                  = CDC_CSCP_CDCClass,
            .SubClass               = CDC_CSCP_ACMSubclass,
            .Protocol               = CDC_CSCP_ATCommandProtocol,

            .InterfaceStrIndex      = NO_DESCRIPTOR
        },

    .CDC2_Functional_Header =
        {
            .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalHeader_t), .Type = DTYPE_CSInterface},
            .Subtype                = CDC_DSUBTYPE_CSInterface_Header,

            .CDCSpecification       = VERSION_BCD(01.10),
        },

    .CDC2_Functional_ACM =
        {
            .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalACM_t), .Type = DTYPE_CSInterface},
            .Subtype                = CDC_DSUBTYPE_CSInterface_ACM,

            .Capabilities           = 0x06,
        },

    .CDC2_Functional_Union =
        {
            .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalUnion_t), .Type = DTYPE_CSInterface},
            .Subtype                = CDC_DSUBTYPE_CSInterface_Union,

            .MasterInterfaceNumber  = 2,
            .SlaveInterfaceNumber   = 3,
        },

    .CDC2_NotificationEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = CDC_NOTIFICATION_EPADDR,
            .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_NOTIFICATION_EPSIZE,
            .PollingIntervalMS      = 0xFF
        },

    .CDC2_DCI_Interface =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

            .InterfaceNumber        = 3,
            .AlternateSetting       = 0,

            .TotalEndpoints         = 2,

            .Class                  = CDC_CSCP_CDCDataClass,
            .SubClass               = CDC_CSCP_NoDataSubclass,
            .Protocol               = CDC_CSCP_NoDataProtocol,

            .InterfaceStrIndex      = NO_DESCRIPTOR
        },

    .CDC2_DataOutEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = CDC2_RX_EPADDR,
            .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_TXRX_EPSIZE,
            .PollingIntervalMS      = 0x05
        },

    .CDC2_DataInEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = CDC2_TX_EPADDR,
            .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_TXRX_EPSIZE,
            .PollingIntervalMS      = 0x05
        },
#endif

#if (TRI_VCP_ENABLE)
        .CDC3_IAD =
            {
                .Header                 = {.Size = sizeof(USB_Descriptor_Interface_Association_t), .Type = DTYPE_InterfaceAssociation},

                .FirstInterfaceIndex    = 4,
                .TotalInterfaces        = 2,

                .Class                  = CDC_CSCP_CDCClass,
                .SubClass               = CDC_CSCP_ACMSubclass,
                .Protocol               = CDC_CSCP_ATCommandProtocol,

                .IADStrIndex            = NO_DESCRIPTOR
            },

        .CDC3_CCI_Interface =
            {
                .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

                .InterfaceNumber        = 4,
                .AlternateSetting       = 0,

                .TotalEndpoints         = 1,

                .Class                  = CDC_CSCP_CDCClass,
                .SubClass               = CDC_CSCP_ACMSubclass,
                .Protocol               = CDC_CSCP_ATCommandProtocol,

                .InterfaceStrIndex      = NO_DESCRIPTOR
            },

        .CDC3_Functional_Header =
            {
                .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalHeader_t), .Type = DTYPE_CSInterface},
                .Subtype                = CDC_DSUBTYPE_CSInterface_Header,

                .CDCSpecification       = VERSION_BCD(01.10),
            },

        .CDC3_Functional_ACM =
            {
                .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalACM_t), .Type = DTYPE_CSInterface},
                .Subtype                = CDC_DSUBTYPE_CSInterface_ACM,

                .Capabilities           = 0x06,
            },

        .CDC3_Functional_Union =
            {
                .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalUnion_t), .Type = DTYPE_CSInterface},
                .Subtype                = CDC_DSUBTYPE_CSInterface_Union,

                .MasterInterfaceNumber  = 4,
                .SlaveInterfaceNumber   = 5,
            },

        .CDC3_NotificationEndpoint =
            {
                .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

                .EndpointAddress        = CDC_NOTIFICATION_EPADDR,
                .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize           = CDC_NOTIFICATION_EPSIZE,
                .PollingIntervalMS      = 0xFF
            },

        .CDC3_DCI_Interface =
            {
                .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

                .InterfaceNumber        = 5,
                .AlternateSetting       = 0,

                .TotalEndpoints         = 2,

                .Class                  = CDC_CSCP_CDCDataClass,
                .SubClass               = CDC_CSCP_NoDataSubclass,
                .Protocol               = CDC_CSCP_NoDataProtocol,

                .InterfaceStrIndex      = NO_DESCRIPTOR
            },

        .CDC3_DataOutEndpoint =
            {
                .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

                .EndpointAddress        = CDC3_RX_EPADDR,
                .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize           = CDC_TXRX_EPSIZE,
                .PollingIntervalMS      = 0x05
            },

        .CDC3_DataInEndpoint =
            {
                .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

                .EndpointAddress        = CDC3_TX_EPADDR,
                .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize           = CDC_TXRX_EPSIZE,
                .PollingIntervalMS      = 0x05
            }
#endif
};


#define STR0LEN 4
static uint8_t const String0Desc[STR0LEN] =
{
	STR0LEN, DTYPE_String, 0x09, 0x04
}; //end of String0Desc

#define STR1LEN sizeof("SILICON LABORATORIES INC.")*2
static uint8_t const String1Desc[STR1LEN] =
{
	STR1LEN, DTYPE_String,
	'S', 0,
	'I', 0,
	'L', 0,
	'I', 0,
	'C', 0,
	'O', 0,
	'N', 0,
	' ', 0,
	'L', 0,
	'A', 0,
	'B', 0,
	'O', 0,
	'R', 0,
	'A', 0,
	'T', 0,
	'O', 0,
	'R', 0,
	'I', 0,
	'E', 0,
	'S', 0,
	' ', 0,
	'I', 0,
	'N', 0,
	'C', 0,
	'.', 0
}; //end of String1Desc

#define STR2LEN sizeof("SiM3U1xx CDC Class")*2

static uint8_t const String2Desc[STR2LEN] =
{
	STR2LEN, DTYPE_String,
	'S', 0,
	'i', 0,
	'M', 0,
	'3', 0,
	'U', 0,
	'1', 0,
	'x', 0,
	'x', 0,
	' ', 0,
	'C', 0,
	'D', 0,
	'C', 0,
	' ', 0,
	'C', 0,
	'l', 0,
	'a', 0,
	's', 0,
	's', 0
}; //end of String2Desc


/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void* Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = &DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = &ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (DescriptorNumber)
			{
				case 0x00:
					Address = String0Desc;
					Size    = STR0LEN;
					break;
				case 0x01:
					Address = String1Desc;
					Size    = STR1LEN;
					break;
				case 0x02:
					Address = String2Desc;
					Size    = STR2LEN;
					break;
			}

			break;
	}

	*DescriptorAddress = Address;
	return Size;
}

