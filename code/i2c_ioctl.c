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

#include <stdio.h>
#include <string.h>
#include <fcntl.h> // open function
#include <unistd.h> // close functions
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#ifndef I2C_M_RD
#include <linux/i2c.h>
#endif

/*
*  Global file descriptor used to talk to the I2C bus:
*/
int i2c_fd = -1;

/*
* Returns a new file descriptor for communicating with the I2C bus:
*/
int i2cInit(const char *i2c_fname)
{
    if ((i2c_fd = open(i2c_fname, O_RDWR)) < 0)
	{
        char err[200];
        sprintf(err, "Failed to open('%s') in i2cInit", i2c_fname);
        perror(err);
        return -1;
    }

    return i2c_fd;
}

void i2cClose(void)
{
    close(i2c_fd);
}

/*
* Write to an I2C slave device's register:
*/
int i2cWriten(u8 slave_addr, u8 reg, u8 *data, u8 bytes)
{
    u8 outbuf[1+bytes];

    struct i2c_msg msgs[1];
    struct i2c_rdwr_ioctl_data msgset[1];

    outbuf[0] = reg;
    for(int i=0; i<bytes; i++)
	{
        outbuf[i+1] = data[i];
	}
	
    msgs[0].addr = slave_addr;
    msgs[0].flags = 0;
    msgs[0].len = 1+bytes;
    msgs[0].buf = outbuf;
    
    msgset[0].msgs = msgs;
    msgset[0].nmsgs = 1;


    if (ioctl(i2c_fd, I2C_RDWR, &msgset) < 0)
	{
        perror("Error: Writing to I2C failed. ioctl(I2C_RDWR) in i2cWriten");
        return -1;
    }

    return 0;
}

/*
* Read the given I2C slave device's register and return the read value in `*result`:
*/
int i2cReadn(u8 slave_addr, u8 reg, u8 *result, u8 bytes)
{
    u8 outbuf[1], inbuf[bytes];
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data msgset[1];

    msgs[0].addr = slave_addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = outbuf;

    msgs[1].addr = slave_addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = bytes;
    msgs[1].buf = inbuf;

    msgset[0].msgs = msgs;
    msgset[0].nmsgs = 2;

    outbuf[0] = reg;

    inbuf[0] = 0;
    bzero(&inbuf, bytes);

    *result = 0;
    if (ioctl(i2c_fd, I2C_RDWR, &msgset) < 0)
	{
        perror("Error: Reading from I2C device failed. ioctl(I2C_RDWR) in i2cReadn");
        return -1;
    }
    
    for(int i=0;i<bytes;i+=1)
	{
      result[i] = inbuf[i];
    }

    return 0;
}

