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

#include "gPB.h"

// Include peripheral access modules used in this file
#include <SI32_PBCFG_A_Type.h>
#include <si32_device.h>
#include <SI32_PBSTD_A_Type.h>
#include <SI32_PBHD_A_Type.h>


void pb_enter_default_mode_from_reset(void)
{
  // PB0 Setup (SPI MISO=PB0.6, SCLK=PB0.5,MOSI=PB0.7,CS=PB0.8)
  SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_0, 0x0040);
  SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_0, 0x01A0);
  SI32_PBSTD_A_write_pbskipen(SI32_PBSTD_0, 0xFE1F);

  // PB1 Setup (UART TX = PB1.12, RX = PB1.13)
  SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_1, 0x2000);
  SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_1, 0x1008);
  SI32_PBSTD_A_write_pbskipen(SI32_PBSTD_1, 0x0FFF);

  // Enable Crossbar0 signals & set properties
  SI32_PBCFG_A_enable_xbar0l_peripherals(SI32_PBCFG_0,
                                         SI32_PBCFG_A_XBAR0L_SPI0EN |
                                         SI32_PBCFG_A_XBAR0L_SPI0NSSEN);
  SI32_PBCFG_A_enable_xbar0h_peripherals(SI32_PBCFG_0, SI32_PBCFG_A_XBAR0H_UART0EN);
  SI32_PBCFG_A_enable_crossbar_0(SI32_PBCFG_0);

  // PB2 Setup (SDSW = PB2.9, PB2.10 and PB2.11 are LEDs)
  SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_2, 0x0200);
  SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x0C00);

  // Enable Crossbar1 signals & set properties
  SI32_PBCFG_A_enable_crossbar_1(SI32_PBCFG_0);
}

//---eof------------------------------------------------------------------------
