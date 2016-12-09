// eDisk.c
// Runs on TM4C123
// Mid-level implementation of the solid state disk device
// driver.  Below this is the low level, hardware-specific
// flash memory interface.  Above this is the high level
// file system implementation.
// Daniel and Jonathan Valvano
// August 29, 2016

/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2016

   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2016

   "Embedded Systems: Introduction to the MSP432 Microcontroller",
   ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2016

   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
   ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2016

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include <stdint.h>
#include "eDisk.h"
#include "FlashProgram.h"
#include "../inc/BSP.h"

//*************** eDisk_Init ***********
// Initialize the interface between microcontroller and disk
// Inputs: drive number (only drive 0 is supported)
// Outputs: status
//  RES_OK        0: Successful
//  RES_ERROR     1: Drive not initialized
enum DRESULT eDisk_Init(uint32_t drive){
  // if drive is 0, return RES_OK, otherwise return RES_ERROR
  // for some configurations the physical drive needs initialization
  // however for the internal flash, no initialization is required
  //    so this function doesn't do anything
  if(drive == 0){             // only drive 0 is supported
     return RES_OK;
  }
  return RES_ERROR;

}

//*************** eDisk_ReadSector ***********
// Read 1 sector of 512 bytes from the disk, data goes to RAM
// Inputs: pointer to an empty RAM buffer
//         sector number of disk to read: 0,1,2,...255
// Outputs: result
//  RES_OK        0: Successful
//  RES_ERROR     1: R/W Error
//  RES_WRPRT     2: Write Protected
//  RES_NOTRDY    3: Not Ready
//  RES_PARERR    4: Invalid Parameter
enum DRESULT eDisk_ReadSector(
    uint8_t *buff,     // Pointer to a RAM buffer into which to store
    uint8_t sector){   // sector number to read from
// starting ROM address of the sector is	EDISK_ADDR_MIN + 512*sector
// return RES_PARERR if EDISK_ADDR_MIN + 512*sector > EDISK_ADDR_MAX
// copy 512 bytes from ROM (disk) into RAM (buff)
// **write this function**
	
	int i=0;
	uint32_t start_addr = EDISK_ADDR_MIN + (512 * sector);
	
	volatile uint8_t *start = (uint8_t *)start_addr;
			if(start_addr >= EDISK_ADDR_MAX)
			return RES_PARERR;
	 for (i=0;i<512; i++) {
				buff[i] = *(start +i);
				BSP_LCD_OutUHex2(buff[i], LCD_ORANGE);
	 }
	  BSP_LCD_DrawString(0, 2, "RD OK", LCD_CYAN);   
		return RES_OK;
}

//*************** eDisk_WriteSector ***********
// Write 1 sector of 512 bytes of data to the disk, data comes from RAM
// Inputs: pointer to RAM buffer with information
//         sector number of disk to write: 0,1,2,...,255
// Outputs: result
//  RES_OK        0: Successful
//  RES_ERROR     1: R/W Error
//  RES_WRPRT     2: Write Protected
//  RES_NOTRDY    3: Not Ready
//  RES_PARERR    4: Invalid Parameter
enum DRESULT eDisk_WriteSector(
    const uint8_t *buff,  // Pointer to the data to be written
    uint8_t sector){      // sector number
// starting ROM address of the sector is	EDISK_ADDR_MIN + 512*sector
// return RES_PARERR if EDISK_ADDR_MIN + 512*sector > EDISK_ADDR_MAX
// write 512 bytes from RAM (buff) into ROM (disk)
// you can use Flash_FastWrite or Flash_WriteArray
// **write this function**
  
uint32_t start = EDISK_ADDR_MIN + (512 * sector);
if(start >=EDISK_ADDR_MAX)
			return RES_PARERR;

	if((Flash_WriteArray((uint32_t*)buff, EDISK_ADDR_MIN+(512*sector), 128 ))!= 128) {
					BSP_LCD_DrawString(0, 12, "eDisk_WriteSector Failed", LCD_YELLOW);
					return RES_ERROR;
	}
BSP_LCD_DrawString(0, 1, "WR OK", LCD_RED); 
	return RES_OK;
}

//*************** eDisk_Format ***********
// Erase all files and all data by resetting the flash to all 1's
// Inputs: none
// Outputs: result
//  RES_OK        0: Successful
//  RES_ERROR     1: R/W Error
//  RES_WRPRT     2: Write Protected
//  RES_NOTRDY    3: Not Ready
//  RES_PARERR    4: Invalid Parameter
enum DRESULT eDisk_Format(void){
// erase all flash from EDISK_ADDR_MIN to EDISK_ADDR_MAX
// **write this function**
  uint32_t i = EDISK_ADDR_MIN;
	int ret=0;
	for(i=EDISK_ADDR_MIN; i<=EDISK_ADDR_MAX; i=i+1024) {
				ret = Flash_Erase(i);
				if(ret == ERROR)
					   BSP_LCD_DrawString(0, 8, "Disk Format", LCD_CYAN);
	}
	
	BSP_LCD_DrawString(0, 0, "FMT OK", LCD_CYAN); 
  return RES_OK;
}
