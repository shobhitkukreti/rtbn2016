// eFile.c
// Runs on either TM4C123 or MSP432
// High-level implementation of the file system implementation.
// Daniel and Jonathan Valvano
// August 29, 2016
#include <stdint.h>
#include "eDisk.h"
#include "../inc/BSP.h"

uint8_t Buff[512]; // temporary buffer used during file I/O
uint8_t Directory[256], FAT[256];
int32_t bDirectoryLoaded =0; // 0 means disk on ROM is complete, 1 means RAM version active
uint32_t fileNo=0;


// Return the larger of two integers.
int16_t max(int16_t a, int16_t b){
  if(a > b){
    return a;
  }
  return b;
}
//*****MountDirectory******
// if directory and FAT are not loaded in RAM,
// bring it into RAM from disk
void MountDirectory(void){ 
// if bDirectoryLoaded is 0, 
//    read disk sector 255 and populate Directory and FAT
//    set bDirectoryLoaded=1
// if bDirectoryLoaded is 1, simply return
// **write this function**

	uint8_t tmpbuf[512];
	int x=0;
	if(bDirectoryLoaded!=1) {
		eDisk_ReadSector(tmpbuf, 255);
		for (x=0; x <256; x++) 
				Directory[x] = tmpbuf[x];
		for(;x<512; x++)
				FAT[x-256] = tmpbuf[x];
		bDirectoryLoaded =1;	
		}
	return;	
}

// Return the index of the last sector in the file
// associated with a given starting sector.
// Note: This function will loop forever without returning
// if the file has no end (i.e. the FAT is corrupted).

/* If start=255, the case when first entry in Directory is 255 and file system is unpopulated
return 255.
else
Scan through the FAT linkedlist for a particular file untill you reach the file's
last sector and return
*/
uint8_t lastsector(uint8_t start){
// **write this function**
	int m=0;
	if(start==255)
			return 255;
	else {
			do 
				{
				m = FAT[start];
				if(m==255)
						return start;
				else	
						start =m;
			}while(1);
		}

	
  //return 0; // replace this line
}

// Return the index of the first free sector.
// Note: This function will loop forever without returning
// if a file has no end or if (Directory[255] != 255)
// (i.e. the FAT is corrupted).

/* when lastsector() returns 255, it is the only 
default case when file system is created and first entry in Directory is 255

else

Call lastsector() for each file index in Directory[i] such that we 
find the last sector for each file. Then update the maximum. 
Since it is contiguous allocation, the file with the highest index 
(sector) will have free space right after it (i.e +1)
*/

uint8_t findfreesector(void){
// **write this function**
  int sec, fs= -1, i=0, maxfs = -1;
	
	for (i=0; i < 255; i++) {
		sec = lastsector(Directory[i]);
			if(sec==255) {
				return fs+1;
			}
			else 
					fs = max(fs,sec);
	}
	return maxfs+1; // replace this line
}

// Append a sector index 'n' at the end of file 'num'.
// This helper function is part of OS_File_Append(), which
// should have already verified that there is free space,
// so it always returns 0 (successful).
// Note: This function will loop forever without returning
// if the file has no end (i.e. the FAT is corrupted).

/* Go to End of file ( last sector by scanning all the sectors of file in FAT)
Change it's value from 255 to the new last sector 'n'
*/

uint8_t appendfat(uint8_t num, uint8_t n){
// **write this function**
  int i=0, m=0;
	i = Directory[num];
	if(i==255) {
			Directory[num]=n;
		//	FAT[n]=255; explicitily set to 255 each time disk is formatted and should remain 255
	}
	else {
			do{
			m =FAT[i];
			if(m==255) { // If the FAT sector depicts end of file by checking 255, stop and add new sector info here
					FAT[i]=n; 
				  //FAT[n]=255; explicitily set to 255 each time disk is formatted and should remain 255
					return 0;
			}
			else	// Else use the next sector info stored in FAT[i] to move in the linkedlist
				i=m;
		}while(1);
		}
	
  return 0; // replace this line
}

//********OS_File_New*************
// Returns a file number of a new file for writing
// Inputs: none
// Outputs: number of a new file
// Errors: return 255 on failure or disk full
uint8_t OS_File_New(void){
// **write this function**
  MountDirectory();
	int x=0;
	for(x=0;x<255; x++) {
			if(Directory[x]==255) {
					return x;
			}
	}
  return 255;
}

//********OS_File_Size*************
// Check the size of this file
// Inputs:  num, 8-bit file number, 0 to 254
// Outputs: 0 if empty, otherwise the number of sectors
// Errors:  none
uint8_t OS_File_Size(uint8_t num){
// **write this function**

/* Find First Sector from Directory[num], Scan through the linkedlist 
	to find last sector with value 255. Return x+1, since first sector starts from 0;
*/
  int x=0;
	uint8_t sector = Directory[num];
	if(sector==255)
			return 0;
	
	for(x=0; x !=255; x++) {
			if(FAT[sector]==255)
					return x+1;
	else	
			sector = FAT[sector];
	}
  return 0; // replace this line
}

//********OS_File_Append*************
// Save 512 bytes into the file
// Inputs:  num, 8-bit file number, 0 to 254
//          buf, pointer to 512 bytes of data
// Outputs: 0 if successful
// Errors:  255 on failure or disk full
uint8_t OS_File_Append(uint8_t num, uint8_t buf[512]){
// **write this function**

  MountDirectory();

	uint8_t n = findfreesector();

	if (n==255)
			return 255;
	else {
				if(eDisk_WriteSector(buf,n)!=0)
										return 255;
				
				appendfat(num,n);
						
				
	}
	
  return 0; // replace this line
}

//********OS_File_Read*************
// Read 512 bytes from the file
// Inputs:  num, 8-bit file number, 0 to 254
//          location, logical address, 0 to 254
//          buf, pointer to 512 empty spaces in RAM
// Outputs: 0 if successful
// Errors:  255 on failure because no data

/* Location is logical sector number for a particular file 
	 Scan through the linked list until you get to that sector number
*/
uint8_t OS_File_Read(uint8_t num, uint8_t location,
                     uint8_t buf[512]){
// **write this function**
  int sector=0;
	sector = Directory[num];
	while(location!=0) {
	sector = FAT[sector];
	if(sector==255)
			return 255;
	location--;
	}

	if(eDisk_ReadSector(buf, sector)!=0)
				return 255;
											 
  return 0; // replace this line
}

//********OS_File_Flush*************
// Update working buffers onto the disk
// Power can be removed after calling flush
// Inputs:  none
// Outputs: 0 if success
// Errors:  255 on disk write failure
uint8_t OS_File_Flush(void){
// **write this function**
		uint8_t tmpbuf[512];
	
	int x=0;
	for(x=0; x<256; x++)
			tmpbuf[x] = Directory[x];
	for(;x<512;x++)
			tmpbuf[x] = FAT[x-256];
	
	if(eDisk_WriteSector(tmpbuf, 255)!=0)
			return 255;
	
  return 0; // replace this line
}

//********OS_File_Format*************
// Erase all files and all data
// Inputs:  none
// Outputs: 0 if success
// Errors:  255 on disk write failure
uint8_t OS_File_Format(void){
// call eDiskFormat
// clear bDirectoryLoaded to zero
// **write this function**

	eDisk_Format();
			
  return 0; // replace this line
}
