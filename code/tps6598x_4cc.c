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

#include "tps6598x_4cc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Max number of attempts to read back cmdx register after sending a command
#define MAX_RETRIES 1000

/*
* Helper function to convert "bytes" to integer (Little Endian)
*/
u32 BytesToInt_LE(u8* bytes)
{
    return (u32)(bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24));
}

/*
* Helper function to convert "bytes" to integer (Big Endian)
*/
u32 BytesToInt_BE(u8* bytes)
{
    return (u32)((bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3]);
}

/*
* Helper function to convert integer to "bytes" (Little Endian)
*/
void IntToBytes_LE(u8* bytes, u32 value)
{
    bytes[0] = (value >> 0) & 0xFF;
    bytes[1] = (value >> 8) & 0xFF;
    bytes[2] = (value >> 16) & 0xFF;
    bytes[3] = (value >> 24) & 0xFF;
}

/*
* Helper function to convert integer to "bytes" (Big Endian)
*/
void IntToBytes_BE(u8* bytes, u32 value)
{
    bytes[0] = (value >> 24) & 0xFF;
    bytes[1] = (value >> 16) & 0xFF;
    bytes[2] = (value >> 8) & 0xFF;
    bytes[3] = (value >> 0) & 0xFF;
}

/*
* Read register function
*/
int RegisterRead(u8 reg, u8* result, u8 bytes)
{
    int ret = 0;
    u8 slaveAddr = 0x38;

    ret = i2cReadn(slaveAddr, reg, result, bytes+1);

    return ret;
}

/*
* Write register function
*/
int RegisterWrite(u8 reg, u8* cmd, u8 bytes)
{
    int ret = 0;
    u8 slaveAddr = 0x38;
    u8 data[bytes+1];

    data[0] = bytes;
    memcpy(&data[1], cmd, bytes);

    ret = i2cWriten(slaveAddr, reg, data, bytes+1);

    return ret;
}

/*
*    Result handling of the following functions:
*    tps65987_FlashLoadReadRegions
*    tps65987_flashMemErase
*    tps65987_FlashMemWriteStartAddr
*    tps65987_FlashMemWrite
*    tps65987_FlashMemVerify
*/
void ResultHandling(u8 val)
{
    if(val == 0x00)
    {
        printf("Task completed successfully\r\n");
    }
    else if(val == 0x01)
    {
        printf("Error: Task timed-out or aborted by ABRT request\r\n");
    }
    else if(val == 0x02)
    {
        printf("Error: Reserved failure\r\n");
    }
    else if(val == 0x03)
    {
        printf("Error: Task rejected\r\n");
    }
    else if(val == 0x21)
    {
        printf("Error: Command not supported\r\n");
    }
    else
    {    
        printf("Error: General error (return value %d)\r\n", val);
    }
}

/*
* Write command to USB C configuration channel(cc)
*/
int WriteCmdToCC(u8* cmd, u8* input, u8 ibytes, u8* output, u8 bytes_to_read, u8* bytes_read)
{
    u8 ret = 0;
    u8 result[64] = {0};
	
    // Write DataX (0x09 or 0x11)
    if(ibytes && input)
    {
        RegisterWrite(0x9, input, ibytes);
    }

    // Write CmdX (0x08 or 0x10)
    ret = RegisterWrite(0x8, cmd, 4);
    if (ret)
    {
        return ret;
    }

    // Poll status from cmdx. Expected result is 4 0 0 0 0 
    for(int i=0;i<MAX_RETRIES;i++)
    {
        usleep(1000);
        ret = RegisterRead(0x8, result, 4);
        if (ret)
        {
            return ret;
        }
        else if(result[0] == 4
             && result[1] == 0
             && result[2] == 0
             && result[3] == 0
             && result[4] == 0 )
        {
            ret= 0;
            break;
        }
        else
        {
            ret=-1;
        }
    }
    if(ret)
    {
        printf("Error: Writing to configuration channel timed out\r\n");
        return ret;
    }
    
    ret = RegisterRead(0x9, result, bytes_to_read);

    *bytes_read = result[0];

    memcpy(output, &result[1], bytes_to_read);
    return ret;
}

/**
 * FLrr - Flash Load Read Regions
 *
 * This command loads the address of the flash memory for 
 * the selected region into Output DataX.
 * 
 * Input DataX:
 *   Byte 1: Region Number (bit0 = Region 0, bit 1 = Region 1)
 * Output DataX:
 *   4 Bytes
 */
int tps65987_FlashLoadReadRegions(u8* input, u8 ibytes)
{
    u8 ret = 0;
    u8* cmd = "FLrr";
    u8 output[64];
    u8 obytes;

    printf("Load flash regions\r\n");
    if(ibytes != 1)
    {
        printf("Error: Selected region (0 or 1) is incorrect. (Received: %d)\r\n", ibytes);
        return -1;
    }

    WriteCmdToCC(cmd, input, ibytes, output, 4, &obytes);
    if(ret != 0)
    {
        ResultHandling(ret);
    }
    
    return ret;
}

/**
 * FLem - Flash Memory Erase
 *
 * This command erases the number of segments specified.
 * 
 * Input DataX:
 *   Bytes 1-4: Flash address first sector (32-bit little-endian)
 *   Byte 5: Number of 4KB sectors to erase.
 * Output DataX:
 *   1 Byte
 */
int tps65987_FlashMemErase(u8* input, u8 ibytes)
{
    u8 ret = 0;
    u8* cmd = "FLem";
    u8 output[64];
    u8 obytes;

    printf("Clear flash:\r\n");
    WriteCmdToCC(cmd, input, ibytes, output, 1, &obytes);
    ret = BytesToInt_LE(output);

    if(ret == 0xff)
    {
        printf("Error: Flash is busy\r\n");
    }
    else if(ret == 0xfe)
    {
        printf("Error: Invalid pointer address, cannot erase\r\n");
    }
    else if(ret != 0)
    {
        //0x3 would mean 'unexpected'
        ResultHandling(ret);
    }
	else
	{
		printf("Flash erased successfully\r\n");
	}
    return ret;
}

/**
 * FLad - Flash Memory Write Start Address
 * 
 * This command sets start address in preparation the flash write.
 *
 * Input DataX:
 *   Bytes 1-4: Flash address (treated as 32-bit little-endian value).
 * Output DataX:
 *   1 Bytes
 */
int tps65987_FlashMemWriteStartAddr(u8* input, u8 ibytes)
{
    u8 ret = 0;
    u8* cmd = "FLad";
    u8 output[64];
    u8 obytes;

    printf("Writing flash start address\r\n");
    WriteCmdToCC(cmd, input, ibytes, output, 1, &obytes);
    ret = BytesToInt_LE(output);

    if(ret != 0)
    {
        ResultHandling(ret);
    }

    return ret;
}

/**
 * FLwd - Flash Memory Write
 *
 * This command writes data beginning at the flash start address
 * defined by the 'FLad' Command. The address is auto-incremented.
 *
 * Input DataX:
 *   Bytes 1-64: Up to 64 bytes of flash data
 * Output DataX:
 *   1 Byte
 */
int tps65987_FlashMemWrite(u8* input, u8 ibytes)
{
    u8 ret = 0;
    u8* cmd = "FLwd";
    u8 output[64];
    u8 obytes;

    WriteCmdToCC(cmd, input, ibytes, output,1 , &obytes);
    ret = output[0];

    if(ret == 0xff)
    {
        printf("Error: Flash is busy!\r\n");
    }
    else if(ret != 0)
    {
        ResultHandling(ret);
    }
    
    return ret;
}

/**
 * FLvy - Flash Memory Verify
 * 
 * This command verifies if the patch/configuration is valid.
 *
 * Input DataX:
 *   Bytes 1-4: Flash address of header location (32-bit little-endian)
 * Output DataX:
 *   1 Byte
 */
int tps65987_FlashMemVerify(u8* input, u8 ibytes)
{
    u8 ret = 0;
    u8* cmd = "FLvy";
    u8 output[64];
    u8 obytes;

    WriteCmdToCC(cmd, input, ibytes, output,1 , &obytes);
    ret =  output[0];

    if(ret == 0x01)
    {
        printf("The patch/configuration is not valid!\r\n");
    }
    else if(ret != 0)
    {
        ResultHandling(ret);
    }
        
    return ret;
}

/**
 * GAID - Cold reset request
 * 
 * This command causes a cold restart of the PD Controller
 * processor.
 * 
 * Input DataX:  0 Byte
 * Output DataX: 0 Byte
 */
int tps65987_ColdResetRequest()
{
    u8 ret = 0;
    u8* cmd = "GAID";

    printf("Request a cold reset\r\n");
    ret = WriteCmdToCC(cmd, NULL, 0, NULL,0, NULL);

    return ret;
}

/*
* Write command to usb cc channel
*/
int tps65987_FlashMemRead(u32 address)
{
    u8 ret = 0;
    u8* cmd = "FLrd";
    u8 output[64];
    u8 obytes;
    u8 input[4] = {0};
    u8 ibytes = 4;

    IntToBytes_LE(input, address);

    ret = WriteCmdToCC(cmd, input, ibytes, output,16, &obytes);

    printf("Read from flash:\r\n");
    for(int i=0;i<16;i++)
    {
        printf(" %2x",output[i]);
    }
    printf("\n");

    return ret;
}
