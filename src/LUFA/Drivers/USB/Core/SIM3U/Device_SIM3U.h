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

/** \file
 *  \brief USB Device definitions for the AVR32 UC3 microcontrollers.
 *  \copydetails Group_Device_UC3
 *
 *  \note This file should not be included directly. It is automatically included as needed by the USB driver
 *        dispatch header located in LUFA/Drivers/USB/USB.h.
 */

/** \ingroup Group_Device
 *  \defgroup Group_Device_UC3 Device Management (UC3)
 *  \brief USB Device definitions for the AVR32 UC3 microcontrollers.
 *
 *  Architecture specific USB Device definitions for the Atmel 32-bit UC3 AVR microcontrollers.
 *
 *  @{
 */

#ifndef __USBDEVICE_UC3_H__
#define __USBDEVICE_UC3_H__

	/* Includes: */
		#include "../../../../Common/Common.h"
		#include "../USBController.h"
		#include "../StdDescriptors.h"
		#include "../USBInterrupt.h"
		#include "../Endpoint.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_USB_DRIVER)
			#error Do not include this file directly. Include LUFA/Drivers/USB/USB.h instead.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** \name USB Device Mode Option Masks */
			//@{
			/** Mask for the Options parameter of the \ref USB_Init() function. This indicates that the
			 *  USB interface should be initialized in low speed (1.5Mb/s) mode.
			 *
			 *  \note Restrictions apply on the number, size and type of endpoints which can be used
			 *        when running in low speed mode - please refer to the USB 2.0 specification.
			 */
			#define USB_DEVICE_OPT_LOWSPEED                (1 << 0)

			/** Mask for the Options parameter of the \ref USB_Init() function. This indicates that the
			 *  USB interface should be initialized in full speed (12Mb/s) mode.
			 */
			#define USB_DEVICE_OPT_FULLSPEED               (0 << 0)
			//@}
			#define USE_INTERNAL_SERIAL             NO_DESCRIPTOR

			#define INTERNAL_SERIAL_LENGTH_BITS     0
			#define INTERNAL_SERIAL_START_ADDRESS   0

		/* Function Prototypes: */
			/** Sends a Remote Wakeup request to the host. This signals to the host that the device should
			 *  be taken out of suspended mode, and communications should resume.
			 *
			 *  Typically, this is implemented so that HID devices (mice, keyboards, etc.) can wake up the
			 *  host computer when the host has suspended all USB devices to enter a low power state.
			 *
			 *  \note This function should only be used if the device has indicated to the host that it
			 *        supports the Remote Wakeup feature in the device descriptors, and should only be
			 *        issued if the host is currently allowing remote wakeup events from the device (i.e.,
			 *        the \ref USB_Device_RemoteWakeupEnabled flag is set). When the \c NO_DEVICE_REMOTE_WAKEUP
			 *        compile time option is used, this function is unavailable.
			 *
			 *  \note The USB clock must be running for this function to operate. If the stack is initialized with
			 *        the \ref USB_OPT_MANUAL_PLL option enabled, the user must ensure that the PLL is running
			 *        before attempting to call this function.
			 *
			 *  \see \ref Group_StdDescriptors for more information on the RMWAKEUP feature and device descriptors.
			 */
			void USB_Device_SendRemoteWakeup(void);

		/* Inline Functions: */
			/** Returns the current USB frame number, when in device mode. Every millisecond the USB bus is active (i.e. enumerated to a host)
			 *  the frame number is incremented by one.
			 *
			 *  \return Current USB frame number from the USB controller.
			 */
			static inline uint16_t USB_Device_GetFrameNumber(void) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;
			static inline uint16_t USB_Device_GetFrameNumber(void)
			{
				return SI32_USB_0->FRAME.FRAMENUM;
			}

			#if !defined(NO_SOF_EVENTS)
				/** Enables the device mode Start Of Frame events. When enabled, this causes the
				 *  \ref EVENT_USB_Device_StartOfFrame() event to fire once per millisecond, synchronized to the USB bus,
				 *  at the start of each USB frame when enumerated in device mode.
				 *
				 *  \note Not available when the \c NO_SOF_EVENTS compile time token is defined.
				 */
				static inline void USB_Device_EnableSOFEvents(void) ATTR_ALWAYS_INLINE;
				static inline void USB_Device_EnableSOFEvents(void)
				{
					SI32_USB_A_enable_start_of_frame_interrupt(SI32_USB_0);
				}

				/** Disables the device mode Start Of Frame events. When disabled, this stops the firing of the
				 *  \ref EVENT_USB_Device_StartOfFrame() event when enumerated in device mode.
				 *
				 *  \note Not available when the \c NO_SOF_EVENTS compile time token is defined.
				 */
				static inline void USB_Device_DisableSOFEvents(void) ATTR_ALWAYS_INLINE;
				static inline void USB_Device_DisableSOFEvents(void)
				{
					SI32_USB_A_disable_start_of_frame_interrupt(SI32_USB_0);
				}
			#endif

	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Inline Functions: */
			static inline void USB_Device_SetLowSpeed(void) ATTR_ALWAYS_INLINE;
			static inline void USB_Device_SetLowSpeed(void)
			{
				// Enable Transceiver, low speed
				SI32_USB_A_write_tcontrol (SI32_USB_0, 0x00);
				SI32_USB_A_select_transceiver_low_speed (SI32_USB_0);
				SI32_USB_A_enable_transceiver (SI32_USB_0);

				// Enable clock recovery, single-step mode disabled
				SI32_USB_A_enable_clock_recovery (SI32_USB_0);
				SI32_USB_A_select_clock_recovery_mode_low_speed (SI32_USB_0);
				SI32_USB_A_select_clock_recovery_normal_cal  (SI32_USB_0);
			}
			static inline void USB_Device_SetFullSpeed(void) ATTR_ALWAYS_INLINE;
			static inline void USB_Device_SetFullSpeed(void)
			{
				// Enable Transceiver, fullspeed
				SI32_USB_A_write_tcontrol (SI32_USB_0, 0x00);
				SI32_USB_A_select_transceiver_full_speed (SI32_USB_0);
				SI32_USB_A_enable_transceiver (SI32_USB_0);

				// Enable clock recovery, single-step mode disabled
				SI32_USB_A_enable_clock_recovery (SI32_USB_0);
				SI32_USB_A_select_clock_recovery_mode_full_speed (SI32_USB_0);
				SI32_USB_A_select_clock_recovery_normal_cal  (SI32_USB_0);
			}

			static inline void USB_Device_SetDeviceAddress(const uint8_t Address) ATTR_ALWAYS_INLINE;
			static inline void USB_Device_SetDeviceAddress(const uint8_t Address)
			{
		        SI32_USB_A_write_faddr(SI32_USB_0, (0x7F & Address));
			}

			static inline bool USB_Device_IsAddressSet(void) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;
			static inline bool USB_Device_IsAddressSet(void)
			{
				return (SI32_USB_A_read_faddr(SI32_USB_0)?true:false);
			}
	#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif

/** @} */

