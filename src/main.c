//------------------------------------------------------------------------------
// Copyright (c) 2012 by Silicon Laboratories.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Silicon Laboratories End User
// License Agreement which accompanies this distribution, and is available at
// http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
// Original content and implementation provided by Silicon Laboratories.
//------------------------------------------------------------------------------
// library
#include <stdio.h>
// hal
#include <si32_device.h>
#include <SI32_PBSTD_A_Type.h>
// application
#include "gModes.h"
#include "gCPU.h"
#include "diskio.h"
#include "ff.h"
#include "circular_buffer.h"
extern void app_main(void);



uint8_t sec_buf[512];
FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
BYTE Buff[128];		/* File read buffer */

void die (		/* Stop with dying message */
	FRESULT rc	/* FatFs return value */
)
{
	printf("Failed with rc=%u.\n", rc);
	for (;;) ;
}
DWORD get_fattime (void)
{
	return	  (((DWORD)(2013 - 1980) << 25)	/* Year = 2012 */
			| ((DWORD)1 << 21)				/* Month = 1 */
			| ((DWORD)1 << 16)				/* Day_m = 1*/
			| ((DWORD)0 << 11)				/* Hour = 0 */
			| ((DWORD)0 << 5)				/* Min = 0 */
			| ((DWORD)0 >> 1));				/* Sec = 0 */
}
void fatfs_test()
{
	FRESULT rc;				/* Result code */
	DIR dir;				/* Directory object */
	FILINFO fno;			/* File information object */
	UINT bw, br, i;

	f_mount(0, &Fatfs);		/* Register volume work area (never fails) */

	printf("\nOpen an existing file (test.abp).\n");
	rc = f_open(&Fil, "TEST.ABP", FA_READ);
	if (rc) die(rc);

	printf("\nType the file content.\n");
	//for (;;)
	do
	{
		rc = f_read(&Fil, Buff, sizeof Buff, &br);	/* Read a chunk of file */
		if (rc || !br) break;			/* Error or end of file */
		for (i = 0; i < br/4; i++)		/* Type the data */
			putchar(Buff[i]);
	}while(0);
	if (rc) die(rc);

	printf("\nClose the file.\n");
	rc = f_close(&Fil);
	if (rc) die(rc);

	printf("\nCreate a new file (hello.txt).\n");
	rc = f_open(&Fil, "HELLO.TXT", FA_WRITE | FA_CREATE_ALWAYS);
	if (rc) die(rc);

	printf("\nWrite a text data. (Hello world!)\n");
	rc = f_write(&Fil, "Hello world! I am FATFs!\r\n", 26, &bw);
	if (rc) die(rc);
	printf("%u bytes written.\n", bw);

	printf("\nClose the file.\n");
	rc = f_close(&Fil);
	if (rc) die(rc);

	printf("\nOpen root directory.\n");
	rc = f_opendir(&dir, "");
	if (rc) die(rc);

	printf("\nDirectory listing...\n");
	for (;;) {
		rc = f_readdir(&dir, &fno);		/* Read a directory item */
		if (rc || !fno.fname[0]) break;	/* Error or end of dir */
		if (fno.fattrib & AM_DIR)
			printf("   <dir>  %s\n", fno.fname);
		else
			printf("%8lu  %s\n", fno.fsize, fno.fname);
	}
	if (rc) die(rc);

	printf("\nTest completed.\n");
	while(1);
}

#if 0
extern void uart_configuration(uint32_t baud_rate,uint8_t stop_bits,uint8_t parity,uint8_t data_bits);
/*
 *   baud_rate: baud rate
 *   stop_bits: 0 - 1 stop bit; 1 - 1.5 stop bits; 2 - 2 stop bits
 *   parity: 0 - none; 1 - odd; 2 - even; 3 - mark; 4 - space
 *   data_bits: data bits(5,6,7,8 or 16)
 */
#define BAUD_RATE 115200
void uart_test(void)
{
	uint8_t data_buf[16];
	uart_configuration(115200,0,0,8);
	while(1)
	{
		if(uart_get_data(data_buf,1) == 0)
		{
			uart_send_data(data_buf,1);
		}
	}
}

void led_on(uint32_t value)
{
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, value);
}
void led_off(uint32_t value)
{
	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_2, value);
}
#endif

//==============================================================================
// myApplication.
//==============================================================================
int main()
{
   // msTicks increments every 1ms (1Khz). Driven by boot osc (myCpu.c)
   // _last variables store the last seen state of variables so we know when they have changed
   uint32_t msTicks_last=0xffffffff;

   while(SI32_PBSTD_A_read_pin(SI32_PBSTD_2,8) == 0);

   //printf("hello1\n");
   // Enter the default operating mode for this application
   enter_default_mode_from_reset();
   circular_buffer_init();
   //printf("hello2\n");

#if 0
   fatfs_test();
   disk_initialize(0);
   disk_read(0,sec_buf,0,1);
   while(1);
#endif
   disk_initialize(0);
   app_main();
   while (1)
   {
     // if msTicks has changed
     if (msTicks != msTicks_last)       
     {
           // EVERY 2^10 counts
       if (!(msTicks % 512)) 
       {
         if(msTicks % 1024)
         {
           // TURN ON LED driver (P2.10)
           SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x400);
         }
         else
         {
           // TURN OFF LED driver (P2.10)
           SI32_PBSTD_A_write_pins_high(SI32_PBSTD_2, 0x400);          
         }
       }
       // save current msTicks value as last seen
       msTicks_last = msTicks;       
     }// if msTicks changed
     
   }//while(1)
}

//-eof--------------------------------------------------------------------------

