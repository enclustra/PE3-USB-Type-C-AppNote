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

#include "i2c_ioctl.h"
#include "tps6598x_4cc.h"
#include "tps6598x_update.h"
#include "tps6598x_ptcx.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

//-------------------------------------------------------------------------------------------------
// Directives, typedefs and constants
//-------------------------------------------------------------------------------------------------

#define REG_ADDR_VERSION 0x0f
#define REG_LEN_VERSION 4

char *commands[]=
{
    "getFWVersion",
    "ProgramRAM",
    "ClearFlash",
    "ProgramFlash",
    "VerifyPatch"
};

/*
* Helper function for terminal prints
*/
void PrintHelp()
{
    printf("Help: usage USBCConfig command i2c-device\r\n\n");
    
    printf("- - - - - - - - - - - - - - \r\n\n");
    printf("cmd is one of the following:\r\n\n");
    printf("- - - - - - - - - - - - - - \r\n\n");
    for(int i=0;i<sizeof(commands)/sizeof(char*);i++)
    {
        printf("%s\r\n",commands[i]);
    }
    printf("- - - - - - - - - - - - - - \r\n\n");
    printf("i2cdev has to be formatted as follows:\r\n\n");
    printf("- - - - - - - - - - - - - - \r\n\n");
    printf("/dev/i2c-0\r\n");
    printf("/dev/i2c-1\r\n");
    printf("/dev/i2c-\r\n");
    printf("...");
    
}

/*
* Read Firmware Version
*/
void ReadFirmwareVersion()
{
    u8 result[128] = {0};
	u32 fwVersion = 0;
	
   // Read Firmware Version register (0x0F)
    printf("Read firmware version register (0x0F)...\r\n");
    RegisterRead(REG_ADDR_VERSION, result, REG_LEN_VERSION);
    fwVersion = BytesToInt_LE(&result[1]);
    printf("Firmware Version: %08X\r\n\n", fwVersion);
}

/*
* Programs the RAM. Called by arg = 1
*/
void ProgramRAM()
{ 
    u8 result[128] = {0};
	u32 fwVersion = 0;
	
	printf("Program RAM\r\n");
    Tps65987PatchQuery();
    PatchRAM();
    Tps65987PatchQuery();
    
    RegisterRead(REG_ADDR_VERSION, result, REG_LEN_VERSION);
    fwVersion = BytesToInt_LE(&result[1]);
    printf("Firmware Version: %08X\r\n", fwVersion);
}

/*
* Clears flash. Called by arg = 2
*/
void ClearFlash()
{
    u8 ibytes = 0;
    u8 input[128] = {0};
	u8 ret = 0;
    
    ibytes = 5;
    input[0] = 0x00;
    input[1]= 0x00;
    input[2]= 0x00;
    input[3]= 0x00;
    input[4] = 8;
    ret = tps65987_FlashMemErase(input, ibytes);
	
	if(ret != 0)
    {
        printf("Error: Erasing flash failed\r\n");
    }
}

/*
* Verifies the patch of the flash. Called by arg = 5
*/
void VerifyPatch()
{
    u8 ibytes = 4;
    u8 input[128] = {0};
    u32 output = 0;
	
	printf("Verify patch:\r\n");
    input[1] = 0x20;
    input[0] = 0x00;
    output = tps65987_FlashMemVerify(input, ibytes);
	
	if(output != 0)
	{
	    printf("Patch verified successfully\r\n");
	}
    else
	{
		printf("Error: Patch verification failed (Error Code %d)\r\n", output);
	}
}


/*
* Main function
*/
int main( int argc, char *argv[])
{

    printf("(c) enclustra GmbH, 2022\r\n");
    printf("Running PE3 USB Type-C configuration\r\n");

    u8 commandIndex=-1;
    if(argc<3){
        printf("command required\r\n");
        PrintHelp();
        return -1;
    }
    char* cmd=argv[1];
    for(int i=0;i<sizeof(commands)/sizeof(char*);i++)
    {
        if(0==strcasecmp(cmd, commands[i]))
       {
           commandIndex=i;
       }
    } 
    
    i2cInit(argv[2]);
    
    switch(commandIndex)
    { 
        case 0:    // Read FW Version Registers
            ReadFirmwareVersion();
            break;
        case 1: // Program RAM
            ProgramRAM();
            break;
        case 2: // Clear Flash
            ClearFlash();
            break;
        case 3: // Program Flash
            ProgramFlash(fullBinary);
            break;
        case 4: // Verify Patch
            VerifyPatch();
            break;
        default:
            printf("unknown command  %s\r\n",cmd);
            PrintHelp();
            return -1;
        break;
    }

}
