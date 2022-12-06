/**---------------------------------------------------------------------------------------------------
-- Copyright (c) 2022 by Enclustra GmbH, Switzerland.
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy of
-- this hardware, software, firmware, and associated documentation files (the
-- "Product"), to deal in the Product without restriction, including without
-- limitation the rights to use, copy, modify, merge, publish, distribute,
-- sublicense, and/or sell copies of the Product, and to permit persons to whom the
-- Product is furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Product.
--
-- THE PRODUCT IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
-- INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
-- PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
-- HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
-- OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
-- PRODUCT OR THE USE OR OTHER DEALINGS IN THE PRODUCT.
---------------------------------------------------------------------------------------------------
*/

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------

#include "tps6598x_update.h"

#include "i2c_ioctl.h"
#include "tps6598x_4cc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * This function programs the flash
 */
int ProgramFlash(int mode)
{
    int ret = 0;
    u8 regionDone = 0;
    u8 activeRegion = 2;
    u8 inputSize = 64;
    u8 input[64] = {0};
    u8 result[16] = {0};
    u8 num4KBSectors = 4;
    u32 bootflags = 0;
    u32 fwVersion = 0;
    u32 startAddr = 0;
	u8 errMarker = 0;
	
    FILE *fp;
    char *buff;
    long filelen;
    
    // Read Firmware Version Register (0x0F)
    printf("Reading Firmware Version:\r\n");
	RegisterRead(0x0F, result, 4);
    fwVersion = BytesToInt_LE(&result[1]);
	printf("FW Version: %08X\r\n", fwVersion);

    // Read Bootflag Register (0x2D)
    printf("Reading Bootflags:\r\n");
	RegisterRead(0x2D, result, 12);
    bootflags = BytesToInt_LE(&result[1]);
    printf("Boot Flags: %08X\r\n", bootflags);

    // Check bootflags for patch errors
    if (bootflags & 0x1)
    {
        printf("Error: Failed to update USBC. (patchHeaderErr) \r\n");
        return -1;
    }

    // Determine active region by checking for region0 (0x10) 
    // and region1 (0x20) bits in bootflags
    if (bootflags & 0x20)
    {
        printf("Error: Can not update USBC due to active region error.\n");
        activeRegion = 2;
    }

    while (0x3 != regionDone)
    {
        // Use FLrr to detect flash address of current region. Full binary image starts at 0x00
        if (mode == regionOnly)
        {
            bzero(input, inputSize);
            input[0] = (activeRegion - 1);
            printf("Getting start address for patch\r\n");
			startAddr = tps65987_FlashLoadReadRegions(input, 1);
            if(startAddr < 0)
            {
                printf("Error: Received wrong address for patch\n");
				return -1;
            }
        }
        
        // During first flash update, the headers has to be written, too
        if (mode == fullBinary)
        { 
            num4KBSectors = 10;
            startAddr=0;
        }
        
        bzero(input, inputSize);
        IntToBytes_LE(&input[0], startAddr);
        input[4] = (num4KBSectors);
        if(tps65987_FlashMemErase(input, 5))
        {
            return -1;
        }


        // FLad - flash memory write start address
        bzero(input, inputSize);
        IntToBytes_LE(&input[0], startAddr);
        if(tps65987_FlashMemWriteStartAddr(input, 4))
        {
            printf("Error: Writing start address to flash failed\n");
            return -1;
        }

        // Read configuration file
        printf("Open configuration file 'full_image.bin'\r\n");
		fp = fopen("full_image.bin", "rb");
        if (fp!=NULL)
		{
			fseek(fp, 0, SEEK_END);
			filelen = ftell(fp);
			rewind(fp);

			buff = (char *)malloc(filelen * sizeof(char)); 
			fread(buff, filelen, 1, fp);
			fclose(fp);
			
			int percStore = 0;
			// Send binary data with FLwd; the address is autoincrementing
			printf("Write configuration file 'full_image.bin' to flash\r\n");
			for (u32 chunk = 0; chunk < filelen/64; chunk++) {
				int percentage = (int)((float)chunk/((float)filelen/64.0)*100.0);
				if(percentage%5 == 0
				   && percentage != percStore)
				{
					printf("%d.", percentage);
					fflush(stdout);
					percStore = percentage;
				}
				if(tps65987_FlashMemWrite((u8*)&buff[chunk*64], 64))
				{
					printf("Error: Flash write failed\r\n");
					errMarker = 1;
					break;
				}
			}
			free(buff);
			
			if(errMarker == 1)
			{
				return -1;
			}
			
			// FLvy - Flash Memory Verify; Abort if it fails
			bzero(input, inputSize);
			input[1] = 0x20;
			printf("Verify patch:\r\n");
			ret = tps65987_FlashMemVerify(input, 4);
			if(ret == 0)
			{
				printf("Patch verified successfully\r\n");
			}
			else
			{
				printf("Error: Patch verification failed (Error Code %d)\r\n", ret);
			}

			if (mode == fullBinary)
			{
				regionDone = 0x3;
			}
			else
			{
				regionDone |= activeRegion;
				activeRegion = activeRegion == 1 ? 2 : 1;
				regionDone = 0x3;
			}
		}
		else
		{
			printf("Error: The file 'full_image.bin' does not exist\r\n");
		}
		printf("-----\n");

		// GAID - Cold reset request
		tps65987_ColdResetRequest();
		usleep(1000*2000);

		// Read Firmware Version Register (0x0F)
		RegisterRead(0x0F, result, 4);
		fwVersion = BytesToInt_LE(&result[1]);
		printf("Firmware Version: %08X\r\n", fwVersion);

		// Read Boot Flags Register (0x2D)
		RegisterRead(0x2D, result, 4);
		bootflags = BytesToInt_LE(&result[1]);
		printf("Bootflags: %08X\r\n", bootflags);
	}
	
    return ret;

}
