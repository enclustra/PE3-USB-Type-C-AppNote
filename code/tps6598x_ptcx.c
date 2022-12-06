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
#include "tps6598x_ptcx.h"

#include "i2c_ioctl.h"
#include "tps6598x_4cc.h"
#include "tps6598x_update.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // close, sleep

/*
* This function patches the query (PTCq)
*/ 
void Tps65987PatchQuery()
{
    u8* cmd = "XXXX";
    u8 obytes;
    u8 output[14];
    cmd = "PTCq";
	printf("Patching the Query\r\n");
    WriteCmdToCC(cmd, NULL, 0, output,14, &obytes);
    printf("Wrote the following bytes:\r\n");
	for(int i=0;i<14;i++)
	{
        printf(" %x",output[i]);
    }
}

/*
* This function patches the RAM
* Warning: The patch bundle update requires the
* 'lower region for pbmx based update' file, not the full binary!
*/ 
int PatchRAM()
{
    u8 input[64] = {0};
    u8* cmd = "XXXX";
    u8 ibytes = 0;
    u8 obytes = 0;
    u8 output[64]; // actual number of output bytes depends on command

	u32 fwVersion = 0;
	u32 bootflags = 0;
	
    FILE *fp;
    char *buff;
    long filelen = 0;
    
    // Do a cold reset
    tps65987_ColdResetRequest();
    
    // Wait for the device
    usleep(1000*2000);

    // PTCs - Start Patch Download Sequence 
    cmd = "PTCs";
    bzero(input, 64);
    input[0] = 0x03;
    ibytes = 1;
    printf("Patching the RAM:\r\n");
	WriteCmdToCC(cmd, input, ibytes, output, 4, &obytes);
    printf("Wrote the following bytes:\r\n");
    for(int i=0;i<4;i++)
	{
        printf(" %x",output[i]);
    }
    
    // PTCd - Patch Download
    cmd = "PTCd";
    ibytes = 64;
	
    printf("Open configuration file 'low_region.bin'\r\n");
    // Read the configuration file
    fp = fopen("low_region.bin", "rb");
	if (fp!=NULL)
	{
		fseek(fp, 0, SEEK_END);
		filelen = ftell(fp);
		rewind(fp);

		buff = (char *)malloc(filelen * sizeof(char)); 
		fread(buff, filelen, 1, fp);
		fclose(fp);
		printf("Configuration file sucessfully opened\r\n");
		printf("Write Patch to control channel (CC)\r\n");
		for (u32 chunk = 0; chunk < filelen/64; chunk++)
		{
			WriteCmdToCC(cmd, (u8*)&buff[chunk*64], ibytes, output, 10, &obytes);
			printf("Wrote the following bytes:\r\n");
			for(int i=0;i<10;i++)
			{
				printf(" %2x",output[i]);
			}
			printf("\n");
			if(output[2] == 0x09)
			{
				printf("Error: Downloading RAM Patch failed\r\n");
			}
		}

		free(buff);
		
		// PTCc - Patch Download Complete
		cmd = "PTCc";
		WriteCmdToCC(cmd, NULL, 0, output,4, &obytes);
		
		// Wait for the patch to apply
		usleep(1000*2000);
		printf("Wait for the patch to apply (PTCc)\r\n");
		printf("Wrote the following bytes:\r\n");
		for(int i=0;i<4;i++)
		{
			printf(" %x",output[i]);
		}
		 
		// Read Firmware Version Register (0x0F)
		bzero(input, 64);
		RegisterRead(0x0F, input, 4);
		fwVersion = BytesToInt_LE(&input[1]);
		printf("%s: FW Version: %08X\n", __func__, fwVersion);

		// Read Bootflags Register (0x2D)
		bzero(input, 64);
		RegisterRead(0x2D, input, 12);
		bootflags = BytesToInt_LE(&input[1]);
		printf("%s: Boot Flags: %08X\n", __func__, bootflags);
	}
	else
	{
		printf("Error: The file 'low_region.bin' does not exist\r\n");
	}
    return 0;
}
