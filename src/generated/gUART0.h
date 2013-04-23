//-------------------------------------------------------------------------------
// Copyright (c) 2012 by Silicon Laboratories
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Silicon Laboratories End User
// License Agreement which accompanies this distribution, and is available at
// http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
//
//
// Original content and implementation provided by Silicon Laboratories
//-------------------------------------------------------------------------------

//==============================================================================
// WARNING:
//
// This file is auto-generated by AppBuilder and should not be modified.
// Any hand modifications will be lost if the project is regenerated.
//==============================================================================

#ifndef __GUART0_H__
#define __GUART0_H__
#include <SI32_UART_A_Type.h>
#define UART_MAX_FIFO_COUNT 4

void UART0_IRQHandler(void);
void UART0_enter_default_mode_from_reset(void);
uint8_t uart_get_byte();
void uart_send_byte(uint8_t data);
uint32_t uart_get_data(uint8_t * data);
void uart_configuration(uint32_t baud_rate,uint8_t stop_bits,uint8_t parity,uint8_t data_bits);

#endif //__GUART0_H__
