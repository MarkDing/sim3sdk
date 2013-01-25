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
 *  \brief USB Endpoint definitions for the AVR32 UC3 microcontrollers.
 *  \copydetails Group_EndpointManagement_UC3
 *
 *  \note This file should not be included directly. It is automatically included as needed by the USB driver
 *        dispatch header located in LUFA/Drivers/USB/USB.h.
 */

/** \ingroup Group_EndpointRW
 *  \defgroup Group_EndpointRW_UC3 Endpoint Data Reading and Writing (UC3)
 *  \brief Endpoint data read/write definitions for the Atmel AVR32 UC3 architecture.
 *
 *  Functions, macros, variables, enums and types related to data reading and writing from and to endpoints.
 */

/** \ingroup Group_EndpointPrimitiveRW
 *  \defgroup Group_EndpointPrimitiveRW_UC3 Read/Write of Primitive Data Types (UC3)
 *  \brief Endpoint primitive read/write definitions for the Atmel AVR32 UC3 architecture.
 *
 *  Functions, macros, variables, enums and types related to data reading and writing of primitive data types
 *  from and to endpoints.
 */

/** \ingroup Group_EndpointPacketManagement
 *  \defgroup Group_EndpointPacketManagement_UC3 Endpoint Packet Management (UC3)
 *  \brief Endpoint packet management definitions for the Atmel AVR32 UC3 architecture.
 *
 *  Functions, macros, variables, enums and types related to packet management of endpoints.
 */

/** \ingroup Group_EndpointManagement
 *  \defgroup Group_EndpointManagement_UC3 Endpoint Management (UC3)
 *  \brief Endpoint management definitions for the Atmel AVR32 UC3 architecture.
 *
 *  Functions, macros and enums related to endpoint management when in USB Device mode. This
 *  module contains the endpoint management macros, as well as endpoint interrupt and data
 *  send/receive functions for various data types.
 *
 *  @{
 */

#ifndef __ENDPOINT_UC3_H__
#define __ENDPOINT_UC3_H__

	/* Includes: */
		#include "../../../../Common/Common.h"
		#include "../USBTask.h"
		#include "../USBInterrupt.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_USB_DRIVER)
			#error Do not include this file directly. Include LUFA/Drivers/USB/USB.h instead.
		#endif

	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Macros: */
			#define ENDPOINT_HSB_ADDRESS_SPACE_SIZE            (64 * 1024UL)
		/* Inline Functions: */

		/* Function Prototypes: */
			void Endpoint_ClearEndpoints(void);

		/* External Variables: */
			extern volatile uint32_t usb_ep_selected;
			extern volatile uint8_t* USB_Endpoint_FIFOPos[];
	#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define USB_EPn(x) ((SI32_USBEP_A_Type*)(0x40018880+((x-1)*0x80)))

			#if (!defined(FIXED_CONTROL_ENDPOINT_SIZE) || defined(__DOXYGEN__))
				/** Default size of the default control endpoint's bank, until altered by the control endpoint bank size
				 *  value in the device descriptor. Not available if the \c FIXED_CONTROL_ENDPOINT_SIZE token is defined.
				 */
				#define ENDPOINT_CONTROLEP_DEFAULT_SIZE     64
			#endif

			#if !defined(CONTROL_ONLY_DEVICE) || defined(__DOXYGEN__)
					/** Total number of endpoints (including the default control endpoint at address 0) which may
					 *  be used in the device. Different AVR models support different amounts of endpoints,
					 *  this value reflects the maximum number of endpoints for the currently selected AVR model.
					 */
				#define ENDPOINT_TOTAL_ENDPOINTS        5
			#else
				#define ENDPOINT_TOTAL_ENDPOINTS            1
			#endif

		/* Enums: */
			/** Enum for the possible error return codes of the \ref Endpoint_WaitUntilReady() function.
			 *
			 *  \ingroup Group_EndpointRW_UC3
			 */
			enum Endpoint_WaitUntilReady_ErrorCodes_t
			{
				ENDPOINT_READYWAIT_NoError                 = 0, /**< Endpoint is ready for next packet, no error. */
				ENDPOINT_READYWAIT_EndpointStalled         = 1, /**< The endpoint was stalled during the stream
				                                                 *   transfer by the host or device.
				                                                 */
				ENDPOINT_READYWAIT_DeviceDisconnected      = 2,	/**< Device was disconnected from the host while
				                                                 *   waiting for the endpoint to become ready.
				                                                 */
				ENDPOINT_READYWAIT_BusSuspended            = 3, /**< The USB bus has been suspended by the host and
				                                                 *   no USB endpoint traffic can occur until the bus
				                                                 *   has resumed.
				                                                 */
				ENDPOINT_READYWAIT_Timeout                 = 4, /**< The host failed to accept or send the next packet
				                                                 *   within the software timeout period set by the
				                                                 *   \ref USB_STREAM_TIMEOUT_MS macro.
				                                                 */
			};

		/* Inline Functions: */
			/** Configures the specified endpoint address with the given endpoint type, direction, bank size
			 *  and banking mode. Once configured, the endpoint may be read from or written to, depending
			 *  on its direction.
			 *
			 *  \param[in] Address    Endpoint address to configure.
			 *
			 *  \param[in] Type       Type of endpoint to configure, a \c EP_TYPE_* mask. Not all endpoint types
			 *                        are available on Low Speed USB devices - refer to the USB 2.0 specification.
			 *
			 *  \param[in] Size       Size of the endpoint's bank, where packets are stored before they are transmitted
			 *                        to the USB host, or after they have been received from the USB host (depending on
			 *                        the endpoint's data direction). The bank size must indicate the maximum packet size
			 *                        that the endpoint can handle.
			 *
			 *  \param[in] Banks      Number of hardware banks to use for the endpoint being configured.
			 *
			 *  \attention When the \c ORDERED_EP_CONFIG compile time option is used, Endpoints <b>must</b> be configured in
			 *             ascending order, or bank corruption will occur.
			 *
			 *  \note Different endpoints may have different maximum packet sizes based on the endpoint's index - refer to
			 *        the chosen microcontroller model's datasheet to determine the maximum bank size for each endpoint.
			 *        \n\n
			 *
			 *  \note The default control endpoint should not be manually configured by the user application, as
			 *        it is automatically configured by the library internally.
			 *        \n\n
			 *
			 *  \note This routine will automatically select the specified endpoint upon success. Upon failure, the endpoint
			 *        which failed to reconfigure correctly will be selected.
			 *
			 *  \return Boolean \c true if the configuration succeeded, \c false otherwise.
			 */
			 bool Endpoint_ConfigureEndpoint(const uint8_t Address,
			                                              const uint8_t Type,
			                                              const uint16_t Size,
			                                              const uint8_t Banks);

			/** Indicates the number of bytes currently stored in the current endpoint's selected bank.
			 *
			 *  \ingroup Group_EndpointRW_UC3
			 *
			 *  \return Total number of bytes in the currently selected Endpoint's FIFO buffer.
			 */
			static inline uint16_t Endpoint_BytesInEndpoint(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline uint16_t Endpoint_BytesInEndpoint(void)
			{
				uint16_t count = 0;
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					if(SI32_USB_A_is_out_packet_ready_ep0(SI32_USB_0))
					{
						count = SI32_USB_0->EP0COUNT.COUNT;
					}
				}
				else
				{
					if(SI32_USBEP_A_is_outpacket_ready(USB_EPn(usb_ep_selected)))
					{
						count = SI32_USBEP_A_read_data_count(USB_EPn(usb_ep_selected));
					}
				}
				return count;
			}

			/** Determines the currently selected endpoint's direction.
			 *
			 *  \return The currently selected endpoint's direction, as a \c ENDPOINT_DIR_* mask.
			 */
			static inline uint8_t Endpoint_GetEndpointDirection(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline uint8_t Endpoint_GetEndpointDirection(void)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
					return ENDPOINT_DIR_IN;
				else
					return (USB_EPn(usb_ep_selected)->EPCONTROL.DIRSEL)?ENDPOINT_DIR_IN : ENDPOINT_DIR_OUT;
			}

			/** Get the endpoint address of the currently selected endpoint. This is typically used to save
			 *  the currently selected endpoint so that it can be restored after another endpoint has been
			 *  manipulated.
			 *
			 *  \return Index of the currently selected endpoint.
			 */
			static inline uint8_t Endpoint_GetCurrentEndpoint(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline uint8_t Endpoint_GetCurrentEndpoint(void)
			{

				return (usb_ep_selected | Endpoint_GetEndpointDirection());
			}

			/** Selects the given endpoint address.
			 *
			 *  Any endpoint operations which do not require the endpoint address to be indicated will operate on
			 *  the currently selected endpoint.
			 *
			 *  \param[in] Address  Endpoint address to select.
			 */
			static inline void Endpoint_SelectEndpoint(const uint8_t Address) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_SelectEndpoint(const uint8_t Address)
			{
				usb_ep_selected = (Address & ENDPOINT_EPNUM_MASK) % ENDPOINT_TOTAL_ENDPOINTS;
			}

			/** Resets the endpoint bank FIFO. This clears all the endpoint banks and resets the USB controller's
			 *  data In and Out pointers to the bank's contents.
			 *
			 *  \param[in] Address  Endpoint number whose FIFO buffers are to be reset.
			 */
			static inline void Endpoint_ResetEndpoint(const uint8_t Address) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_ResetEndpoint(const uint8_t Address)
			{
				if(usb_ep_selected != ENDPOINT_CONTROLEP)
				{
					USB_EPn(usb_ep_selected)->EPCONTROL.U32 |= (1<<20)|(1<<3);
				}
			}

			/** Enables the currently selected endpoint so that data can be sent and received through it to
			 *  and from a host.
			 *
			 *  \note Endpoints must first be configured properly via \ref Endpoint_ConfigureEndpoint().
			 */
			static inline void Endpoint_EnableEndpoint(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_EnableEndpoint(void)
			{
				SI32_USB_0->CMINTEPE.U32 |= (1<<(SI32_USB_A_CMINTEPE_EP0EN_SHIFT + usb_ep_selected));
			}

			/** Disables the currently selected endpoint so that data cannot be sent and received through it
			 *  to and from a host.
			 */
			static inline void Endpoint_DisableEndpoint(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_DisableEndpoint(void)
			{
				SI32_USB_0->CMINTEPE.U32 &= ~(1<<(SI32_USB_A_CMINTEPE_EP0EN_SHIFT + usb_ep_selected));
			}

			/** Determines if the currently selected endpoint is enabled, but not necessarily configured.
			 *
			 * \return Boolean \c true if the currently selected endpoint is enabled, \c false otherwise.
			 */
			static inline bool Endpoint_IsEnabled(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsEnabled(void)
			{
				return ((SI32_USB_0->CMINTEPE.U32 & (1<<(SI32_USB_A_CMINTEPE_EP0EN_SHIFT + usb_ep_selected))) ? true : false);
			}

			/** Retrieves the number of busy banks in the currently selected endpoint, which have been queued for
			 *  transmission via the \ref Endpoint_ClearIN() command, or are awaiting acknowledgement via the
			 *  \ref Endpoint_ClearOUT() command.
			 *
			 *  \ingroup Group_EndpointPacketManagement_UC3
			 *
			 *  \return Total number of busy banks in the selected endpoint.
			 */
			static inline uint8_t Endpoint_GetBusyBanks(void) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Endpoint_GetBusyBanks(void)
			{
				return (0);
			}

			/** Aborts all pending IN transactions on the currently selected endpoint, once the bank
			 *  has been queued for transmission to the host via \ref Endpoint_ClearIN(). This function
			 *  will terminate all queued transactions, resetting the endpoint banks ready for a new
			 *  packet.
			 *
			 *  \ingroup Group_EndpointPacketManagement_UC3
			 */
			static inline void Endpoint_AbortPendingIN(void)
			{

			}

			/** Determines if the selected IN endpoint is ready for a new packet to be sent to the host.
			 *
			 *  \ingroup Group_EndpointPacketManagement_UC3
			 *
			 *  \return Boolean \c true if the current endpoint is ready for an IN packet, \c false otherwise.
			 */
			static inline bool Endpoint_IsINReady(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsINReady(void)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					return (SI32_USB_0->EP0CONTROL.IPRDYI?false:true);
				}
				else
				{
					return (USB_EPn(usb_ep_selected)->EPCONTROL.IPRDYI?false:true);
				}
			}
			/** Determines if the selected OUT endpoint has received new packet from the host.
			 *
			 *  \ingroup Group_EndpointPacketManagement_UC3
			 *
			 *  \return Boolean \c true if current endpoint is has received an OUT packet, \c false otherwise.
			 */
			static inline bool Endpoint_IsOUTReceived(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsOUTReceived(void)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					return (SI32_USB_A_is_out_packet_ready_ep0(SI32_USB_0)); // OPRDYI;
				}
				else
				{
					return (USB_EPn(usb_ep_selected)->EPCONTROL.OPRDYI?true:false);
				}
			}
			/** Determines if the currently selected endpoint may be read from (if data is waiting in the endpoint
			 *  bank and the endpoint is an OUT direction, or if the bank is not yet full if the endpoint is an IN
			 *  direction). This function will return false if an error has occurred in the endpoint, if the endpoint
			 *  is an OUT direction and no packet (or an empty packet) has been received, or if the endpoint is an IN
			 *  direction and the endpoint bank is full.
			 *
			 *  \ingroup Group_EndpointPacketManagement_UC3
			 *
			 *  \return Boolean \c true if the currently selected endpoint may be read from or written to, depending
			 *          on its direction.
			 */
			static inline bool Endpoint_IsReadWriteAllowed(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsReadWriteAllowed(void)
			{
				bool status = false;
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					status = true;
				}
				else
				{
					if(Endpoint_GetEndpointDirection() == ENDPOINT_DIR_IN)
					{
						status = Endpoint_IsINReady();
					}
					else
					{
						status = Endpoint_IsOUTReceived();
					}
				}
				return status;
			}

			/** Determines if the currently selected endpoint is configured.
			 *
			 *  \return Boolean \c true if the currently selected endpoint has been configured, \c false otherwise.
			 */
			static inline bool Endpoint_IsConfigured(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsConfigured(void)
			{
				return (true);
			}

			/** Returns a mask indicating which INTERRUPT type endpoints have interrupted - i.e. their
			 *  interrupt duration has elapsed. Which endpoints have interrupted can be determined by
			 *  masking the return value against <tt>(1 << <i>{Endpoint Number}</i>)</tt>.
			 *
			 *  \return Mask whose bits indicate which endpoints have interrupted.
			 */
			static inline uint32_t Endpoint_GetEndpointInterrupts(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline uint32_t Endpoint_GetEndpointInterrupts(void)
			{
				return(0);
			}

			/** Determines if the specified endpoint number has interrupted (valid only for INTERRUPT type
			 *  endpoints).
			 *
			 *  \param[in] Address  Address of the endpoint whose interrupt flag should be tested.
			 *
			 *  \return Boolean \c true if the specified endpoint has interrupted, \c false otherwise.
			 */
			static inline bool Endpoint_HasEndpointInterrupted(const uint8_t Address) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_HasEndpointInterrupted(const uint8_t Address)
			{
				return 0;
			}


			/** Determines if the current CONTROL type endpoint has received a SETUP packet.
			 *
			 *  \ingroup Group_EndpointPacketManagement_UC3
			 *
			 *  \return Boolean \c true if the selected endpoint has received a SETUP packet, \c false otherwise.
			 */
			static inline bool Endpoint_IsSETUPReceived(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsSETUPReceived(void)
			{
				return SI32_USB_A_is_out_packet_ready_ep0(SI32_USB_0); // OPRDYI
			}

			/** Clears a received SETUP packet on the currently selected CONTROL type endpoint, freeing up the
			 *  endpoint for the next packet.
			 *
			 *  \ingroup Group_EndpointPacketManagement_UC3
			 *
			 *  \note This is not applicable for non CONTROL type endpoints.
			 */
			static inline void Endpoint_ClearSETUP(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_ClearSETUP(void)
			{
				SI32_USB_A_clear_out_packet_ready_ep0(SI32_USB_0); // OPRDYIS = 1
			}

			/** Sends an IN packet to the host on the currently selected endpoint, freeing up the endpoint for the
			 *  next packet and switching to the alternative endpoint bank if double banked.
			 *
			 *  \ingroup Group_EndpointPacketManagement_UC3
			 */
			static inline void Endpoint_ClearIN(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_ClearIN(void)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
		            SI32_USB_A_set_data_end_ep0(SI32_USB_0);
		            SI32_USB_A_set_in_packet_ready_ep0(SI32_USB_0);
				}
				else
				{
					USB_EPn(usb_ep_selected)->EPCONTROL.IPRDYI = 1;
				}
			}

			/** Acknowledges an OUT packet to the host on the currently selected endpoint, freeing up the endpoint
			 *  for the next packet and switching to the alternative endpoint bank if double banked.
			 *
			 *  \ingroup Group_EndpointPacketManagement_UC3
			 */
			static inline void Endpoint_ClearOUT(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_ClearOUT(void)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					SI32_USB_A_clear_out_packet_ready_ep0(SI32_USB_0); // OPRDYIS = 1
				}
				else
				{

					USB_EPn(usb_ep_selected)->EPCONTROL.OPRDYI = 0;
				}
			}

			/** Stalls the current endpoint, indicating to the host that a logical problem occurred with the
			 *  indicated endpoint and that the current transfer sequence should be aborted. This provides a
			 *  way for devices to indicate invalid commands to the host so that the current transfer can be
			 *  aborted and the host can begin its own recovery sequence.
			 *
			 *  The currently selected endpoint remains stalled until either the \ref Endpoint_ClearStall() macro
			 *  is called, or the host issues a CLEAR FEATURE request to the device for the currently selected
			 *  endpoint.
			 *
			 *  \ingroup Group_EndpointPacketManagement_UC3
			 */
			static inline void Endpoint_StallTransaction(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_StallTransaction(void)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					SI32_USB_A_send_stall_ep0(SI32_USB_0);
				}
				else
				{
					if(Endpoint_GetEndpointDirection()== ENDPOINT_DIR_IN)
					{
						USB_EPn(usb_ep_selected)->EPCONTROL.ISDSTL = 1 ;
					}
					else
					{
						USB_EPn(usb_ep_selected)->EPCONTROL.OSDSTL = 1;
					}
				}
			}

			/** Clears the STALL condition on the currently selected endpoint.
			 *
			 *  \ingroup Group_EndpointPacketManagement_UC3
			 */
			static inline void Endpoint_ClearStall(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_ClearStall(void)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					SI32_USB_A_clear_stall_sent_ep0(SI32_USB_0);
				}
				else
				{
					if(Endpoint_GetEndpointDirection()== ENDPOINT_DIR_IN)
					{
						USB_EPn(usb_ep_selected)->EPCONTROL.ISDSTL = 0 ;
						USB_EPn(usb_ep_selected)->EPCONTROL.ISTSTLI = 0 ;
					}
					else
					{
						USB_EPn(usb_ep_selected)->EPCONTROL.OSTSTLI = 0;
						USB_EPn(usb_ep_selected)->EPCONTROL.OSDSTL = 0;
					}
				}
			}

			/** Determines if the currently selected endpoint is stalled, false otherwise.
			 *
			 *  \ingroup Group_EndpointPacketManagement_UC3
			 *
			 *  \return Boolean \c true if the currently selected endpoint is stalled, \c false otherwise.
			 */
			static inline bool Endpoint_IsStalled(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline bool Endpoint_IsStalled(void)
			{
				bool status = false ;
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					status = SI32_USB_A_is_stall_sent_ep0(SI32_USB_0);
				}
				else
				{
					if(Endpoint_GetEndpointDirection()== ENDPOINT_DIR_IN)
					{
						status = USB_EPn(usb_ep_selected)->EPCONTROL.ISDSTL;
					}
					else
					{
						status = USB_EPn(usb_ep_selected)->EPCONTROL.OSDSTL;
					}
				}
				return status;
			}

			/** Resets the data toggle of the currently selected endpoint. */
			static inline void Endpoint_ResetDataToggle(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_ResetDataToggle(void)
			{
				if(usb_ep_selected != ENDPOINT_CONTROLEP)
				{
					USB_EPn(usb_ep_selected)->EPCONTROL.OCLRDT = 1;
					USB_EPn(usb_ep_selected)->EPCONTROL.ICLRDT = 1;
				}
			}

			/** Sets the direction of the currently selected endpoint.
			 *
			 *  \param[in] DirectionMask  New endpoint direction, as a \c ENDPOINT_DIR_* mask.
			 */
			static inline void Endpoint_SetEndpointDirection(const uint32_t DirectionMask) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_SetEndpointDirection(const uint32_t DirectionMask)
			{
				return;
			}

			/** Reads one byte from the currently selected endpoint's bank, for OUT direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 *
			 *  \return Next byte in the currently selected endpoint's FIFO buffer.
			 */
			static inline uint8_t Endpoint_Read_8(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline uint8_t Endpoint_Read_8(void)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					return SI32_USB_A_read_ep0_fifo_u8(SI32_USB_0);
				}
				else
				{
					return SI32_USBEP_A_read_fifo_u8(USB_EPn(usb_ep_selected));
				}
			}

			/** Writes one byte to the currently selected endpoint's bank, for IN direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 *
			 *  \param[in] Data  Data to write into the the currently selected endpoint's FIFO buffer.
			 */
			static inline void Endpoint_Write_8(const uint8_t Data) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_Write_8(const uint8_t Data)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					SI32_USB_A_write_ep0_fifo_u8(SI32_USB_0,Data);
				}
				else
				{
					SI32_USBEP_A_write_fifo_u8(USB_EPn(usb_ep_selected),Data);
				}
			}

			/** Discards one byte from the currently selected endpoint's bank, for OUT direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 */
			static inline void Endpoint_Discard_8(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_Discard_8(void)
			{
				volatile uint8_t Dummy;
				Dummy = Endpoint_Read_8();
				(void)Dummy;
			}

			/** Reads two bytes from the currently selected endpoint's bank in little endian format, for OUT
			 *  direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 *
			 *  \return Next two bytes in the currently selected endpoint's FIFO buffer.
			 */
			static inline uint16_t Endpoint_Read_16_LE(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline uint16_t Endpoint_Read_16_LE(void)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					return SI32_USB_A_read_ep0_fifo_u16(SI32_USB_0);
				}
				else
				{
					return SI32_USBEP_A_read_fifo_u16(USB_EPn(usb_ep_selected));
				}
			}

			/** Reads two bytes from the currently selected endpoint's bank in big endian format, for OUT
			 *  direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 *
			 *  \return Next two bytes in the currently selected endpoint's FIFO buffer.
			 */
			static inline uint16_t Endpoint_Read_16_BE(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline uint16_t Endpoint_Read_16_BE(void)
			{
				return (0);
			}

			/** Writes two bytes to the currently selected endpoint's bank in little endian format, for IN
			 *  direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 *
			 *  \param[in] Data  Data to write to the currently selected endpoint's FIFO buffer.
			 */
			static inline void Endpoint_Write_16_LE(const uint16_t Data) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_Write_16_LE(const uint16_t Data)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					SI32_USB_A_write_ep0_fifo_u16(SI32_USB_0,Data);
				}
				else
				{
					SI32_USBEP_A_write_fifo_u16(USB_EPn(usb_ep_selected),Data);
				}
			}

			/** Writes two bytes to the currently selected endpoint's bank in big endian format, for IN
			 *  direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 *
			 *  \param[in] Data  Data to write to the currently selected endpoint's FIFO buffer.
			 */
			static inline void Endpoint_Write_16_BE(const uint16_t Data) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_Write_16_BE(const uint16_t Data)
			{
			}

			/** Discards two bytes from the currently selected endpoint's bank, for OUT direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 */
			static inline void Endpoint_Discard_16(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_Discard_16(void)
			{
				volatile uint16_t Dummy;
				Dummy = Endpoint_Read_16_LE();
				(void)Dummy;
			}

			/** Reads four bytes from the currently selected endpoint's bank in little endian format, for OUT
			 *  direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 *
			 *  \return Next four bytes in the currently selected endpoint's FIFO buffer.
			 */
			static inline uint32_t Endpoint_Read_32_LE(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline uint32_t Endpoint_Read_32_LE(void)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					return SI32_USB_A_read_ep0_fifo_u32(SI32_USB_0);
				}
				else
				{
					return SI32_USBEP_A_read_fifo_u32(USB_EPn(usb_ep_selected));
				}
			}

			/** Reads four bytes from the currently selected endpoint's bank in big endian format, for OUT
			 *  direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 *
			 *  \return Next four bytes in the currently selected endpoint's FIFO buffer.
			 */
			static inline uint32_t Endpoint_Read_32_BE(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
			static inline uint32_t Endpoint_Read_32_BE(void)
			{
				return 0;
			}

			/** Writes four bytes to the currently selected endpoint's bank in little endian format, for IN
			 *  direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 *
			 *  \param[in] Data  Data to write to the currently selected endpoint's FIFO buffer.
			 */
			static inline void Endpoint_Write_32_LE(const uint32_t Data) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_Write_32_LE(const uint32_t Data)
			{
				if(usb_ep_selected == ENDPOINT_CONTROLEP)
				{
					SI32_USB_A_write_ep0_fifo_u32(SI32_USB_0,Data);
				}
				else
				{
					SI32_USBEP_A_write_fifo_u32(USB_EPn(usb_ep_selected),Data);
				}
			}

			/** Writes four bytes to the currently selected endpoint's bank in big endian format, for IN
			 *  direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 *
			 *  \param[in] Data  Data to write to the currently selected endpoint's FIFO buffer.
			 */
			static inline void Endpoint_Write_32_BE(const uint32_t Data) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_Write_32_BE(const uint32_t Data)
			{
			}

			/** Discards four bytes from the currently selected endpoint's bank, for OUT direction endpoints.
			 *
			 *  \ingroup Group_EndpointPrimitiveRW_UC3
			 */
			static inline void Endpoint_Discard_32(void) ATTR_ALWAYS_INLINE;
			static inline void Endpoint_Discard_32(void)
			{
				volatile uint32_t Dummy;
				Dummy = Endpoint_Read_16_LE();
				(void)Dummy;
			}

		/* External Variables: */
			/** Global indicating the maximum packet size of the default control endpoint located at address
			 *  0 in the device. This value is set to the value indicated in the device descriptor in the user
			 *  project once the USB interface is initialized into device mode.
			 *
			 *  If space is an issue, it is possible to fix this to a static value by defining the control
			 *  endpoint size in the \c FIXED_CONTROL_ENDPOINT_SIZE token passed to the compiler in the makefile
			 *  via the -D switch. When a fixed control endpoint size is used, the size is no longer dynamically
			 *  read from the descriptors at runtime and instead fixed to the given value. When used, it is
			 *  important that the descriptor control endpoint size value matches the size given as the
			 *  \c FIXED_CONTROL_ENDPOINT_SIZE token - it is recommended that the \c FIXED_CONTROL_ENDPOINT_SIZE token
			 *  be used in the device descriptors to ensure this.
			 *
			 *  \attention This variable should be treated as read-only in the user application, and never manually
			 *             changed in value.
			 */
			#if (!defined(FIXED_CONTROL_ENDPOINT_SIZE) || defined(__DOXYGEN__))
				extern uint8_t USB_Device_ControlEndpointSize;
			#else
				#define USB_Device_ControlEndpointSize FIXED_CONTROL_ENDPOINT_SIZE
			#endif

		/* Function Prototypes: */
			/** Configures a table of endpoint descriptions, in sequence. This function can be used to configure multiple
			 *  endpoints at the same time.
			 *
			 *  \note Endpoints with a zero address will be ignored, thus this function cannot be used to configure the
			 *        control endpoint.
			 *
			 *  \param[in] Table    Pointer to a table of endpoint descriptions.
			 *  \param[in] Entries  Number of entries in the endpoint table to configure.
			 *
			 *  \return Boolean \c true if all endpoints configured successfully, \c false otherwise.
			 */
			bool Endpoint_ConfigureEndpointTable(const USB_Endpoint_Table_t* const Table,
			                                     const uint8_t Entries);

			/** Completes the status stage of a control transfer on a CONTROL type endpoint automatically,
			 *  with respect to the data direction. This is a convenience function which can be used to
			 *  simplify user control request handling.
			 *
			 *  \note This routine should not be called on non CONTROL type endpoints.
			 */
			void Endpoint_ClearStatusStage(void);

			/** Spin-loops until the currently selected non-control endpoint is ready for the next packet of data
			 *  to be read or written to it.
			 *
			 *  \note This routine should not be called on CONTROL type endpoints.
			 *
			 *  \ingroup Group_EndpointRW_UC3
			 *
			 *  \return A value from the \ref Endpoint_WaitUntilReady_ErrorCodes_t enum.
			 */
			uint8_t Endpoint_WaitUntilReady(void);

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif

/** @} */

