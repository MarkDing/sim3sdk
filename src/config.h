//------------------------------------------------------------------------------
// Copyright (c) 2012 by Silicon Laboratories.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Silicon Laboratories End User
// License Agreement which accompanies this distribution, and is available at
// http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
// Original content and implementation provided by Silicon Laboratories.
//------------------------------------------------------------------------------
#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <stdint.h>
/* macros definition */
#define SYSCLK                     48000000    // SYSCLK frequency in Hz
#define APBCLK                     (SYSCLK)
#define TIMER0_BASE_FREQ           4000000
#define TIMER1_BASE_FREQ           4000000

#define TURN_OFF_INTERRUPTS         { __disable_irq(); }
#define TURN_ON_INTERRUPTS          { __enable_irq(); }


#define USART0_InterruptPriority    1
#define USART1_InterruptPriority    1
#define UART0_InterruptPriority     1
#define UART1_InterruptPriority     0

#define USB0_InterruptPriority      2

#define PMATCH_InterruptPriority    2
#define PCA0_InterruptPriority      1
#define PCA1_InterruptPriority      1
#define EPCA0_InterruptPriority     3

#define TIMER0L_InterruptPriority   2
#define TIMER0H_InterruptPriority   2
#define TIMER1L_InterruptPriority   2
#define TIMER1H_InterruptPriority   2


// Define Endpoint Packet Sizes
#define  EP0_PACKET_SIZE         0x40
#define  EP1_PACKET_SIZE         0x0010
#define  EP2_PACKET_SIZE         0x0040
#define  EP3_PACKET_SIZE         0x0040
#define  EP4_PACKET_SIZE         0x0040


#define  EPn_NUMBER   4

#endif
