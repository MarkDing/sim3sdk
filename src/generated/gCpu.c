//------------------------------------------------------------------------------
// Copyright (c) 2012 by Silicon Laboratories.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Silicon Laboratories End User
// License Agreement which accompanies this distribution, and is available at
// http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
// Original content and implementation provided by Silicon Laboratories.
//------------------------------------------------------------------------------

//==============================================================================
// WARNING:
//
// This file is auto-generated by AppBuilder and should not be modified.
// Any hand modifications will be lost if the project is regenerated.
//==============================================================================
// library
#include <stdio.h>
#include <si32_device.h>
// hal
#include <si32_device.h>
#include <SI32_CLKCTRL_A_Type.h>
#include <SI32_PBSTD_A_Type.h>
#include <SI32_WDTIMER_A_Type.h>

#ifdef DEBUG
#define printf(...)
#endif

void set_ahb_clock(uint32_t freq);
void halt(void);

//------------------------------------------------------------------------------
// This function is invoked by the CMSIS requires SysemInit() function in
// system_<device>.c.  SystemInit() is invoked by Reset_Handler() when the
// CPU boots.
void mySystemInit()
{
   // disable the watchdog timer to prevent device resets
   // any lines added to this function should be added below this line of code
   SI32_WDTIMER_A_stop_counter (SI32_WDTIMER_0);

   // the following lines of code are necessary to enable debug printf
   // note that this pin changes between the different package options

   // enable APB clock to the Port Bank module
   SI32_CLKCTRL_A_enable_apb_to_modules_0 (SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_PB0CEN_MASK);
   // make the SWO pin (PB1.3) push-pull to enable SWV printf
   SI32_PBSTD_A_set_pins_push_pull_output (SI32_PBSTD_1, (1<<3));
}

// MODULE VARIABLES
//------------------------------------------------------------------------------
//Variable that incrments 1000 times / second
volatile uint32_t msTicks;

//==============================================================================
//2nd LEVEL  INTERRUPT HANDLERS
//==============================================================================
/*NONE*/

//==============================================================================
//1st LEVEL  INTERRUPT HANDLERS
//==============================================================================
void SysTick_Handler(void)
{
   msTicks++;
   /*NO SECOND LEVEL HANDLER SPECIFIED*/
}

//------------------------------------------------------------------------------
void NMI_Handler(void)
{
  printf("NMI_Handler\n");
  /*NO SECOND LEVEL HANDLER SPECIFIED (halt USED)*/
  halt();
}

//------------------------------------------------------------------------------
void HardFault_Handler(void)
{
  printf("HardFault_Handler\n");
  /*NO SECOND LEVEL HANDLER SPECIFIED (halt USED)*/
  halt();
}

//------------------------------------------------------------------------------
void MemManage_Handler(void)
{
  printf("MemManage_Handler\n");
  /*NO SECOND LEVEL HANDLER SPECIFIED (halt USED)*/
  halt();
}

//------------------------------------------------------------------------------
void BusFault_Handler(void)
{
  printf("BusFault_Handler\n");
  /*NO SECOND LEVEL HANDLER SPECIFIED (halt USED)*/
  halt();
}

//------------------------------------------------------------------------------
void UsageFault_Handler(void)
{
  printf("UsageFault_Handler\n");
  /*NO SECOND LEVEL HANDLER SPECIFIED (halt USED)*/
  halt();
}


//==============================================================================
//CONFIGURATION FUNCTIONS
//==============================================================================
// Sets up systick
void cpu_update(void)
{
  set_ahb_clock(SystemCoreClock);

# if SI32_BASE_CPU_ARMV7M
  // set Priority for Cortex-M0 System Interrupts.
  NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
  // TBD the rest of them
# endif
}

//==============================================================================
// SUPPORT FUNCTIONS
//==============================================================================
void set_ahb_clock(uint32_t freq)
{
  // UPDATE SYSTICK
  if (SysTick_Config(freq / 1000))
  {
    printf("ERROR: SysTick_Config failed\n");
  }

  // UPDATE ITM DIVIDER
  *((uint32_t *) 0xE0040010) = ((50 * freq) / 20000000) - 1;
}// set_ahb_clock

//------------------------------------------------------------------------------
uint32_t get_msTicks(void)
{
  return msTicks;
}// get_msTicks

//------------------------------------------------------------------------------
void halt(void)
{
  printf("Halted\n");
  // Configurable Fault Status Register
  printf(" CFSR: 0x%08X\n", SCB->CFSR);
  // Hard Fault Status Register
  printf(" HFSR: 0x%08X\n", SCB->HFSR);
  // Debug Fault Status Register
  printf(" DFSR: 0x%08X\n", SCB->DFSR);
  // Memory Management Fault Address Register
  printf("MMFAR: 0x%08X\n", SCB->MMFAR);
  // Bus Fault Address register
  printf(" BFAR: 0x%08X\n", SCB->BFAR);
  //Auxiliary Fault Status Register
  printf(" AFSR: 0x%08X\n", SCB->AFSR);
  while(1);
}

void gCpu_enter_default_config(void)
{
  SystemCoreClock = 20000000;
  if (SysTick_Config(SystemCoreClock / 1000))
  {
    printf("ERROR: SysTick_Config failed\n");
  }

  // set Priority for Cortex-M0 System Interrupts.
  NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

//-eof--------------------------------------------------------------------------
