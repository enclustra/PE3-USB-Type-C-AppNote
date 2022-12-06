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

void ResultHandling(u8 val);

// CC commands
int tps65987_ColdResetRequest();                           // GAID
int tps65987_FlashMemRead();                               // FLrd
int tps65987_FlashMemVerify(u8* input, u8 ibytes);         // FLvy
int tps65987_FlashMemWrite(u8* input, u8 ibytes);          // FLwd
int tps65987_FlashMemWriteStartAddr(u8* input, u8 ibytes); // FLad
int tps65987_FlashMemErase(u8* input, u8 ibytes);          // FLem
int tps65987_FlashLoadReadRegions(u8* input, u8 ibytes);   // FLrr


// CC read / write functions
int WriteCmdToCC(u8* cmd, u8* input, u8 ibytes, u8* output, u8 bytes_to_read, u8* bytes_read);
int RegisterRead(u8 reg, u8* result, u8 bytes);
int RegisterWrite(u8 reg, u8* cmd, u8 bytes);


// helper functions (conversion)
u32 BytesToInt_LE(u8* bytes);
u32 BytesToInt_BE(u8* bytes);
void IntToBytes_LE(u8* bytes, u32 value);
void IntToBytes_BE(u8* bytes, u32 value);
