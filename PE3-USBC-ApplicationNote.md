# PE3 USB Type-C Application Note

## Purpose

This application note provides information about the functions of USB Type-C
on Enclustra PE3 hardware.

## Summary

This application note gives an overview over the setup process for USB Type-C on Enclustra PE3 hardware.
This includes configuring the USB Type-C controller via the controller config tool (TPS65981_2_7_8 Application Customization Tool)

## License

Copyright 2022 by Enclustra GmbH, Switzerland.

Permission is hereby granted, free of charge, to any person obtaining a copy of this hardware, software,
firmware, and associated documentation files (the ”Product”), to deal in the Product without restriction, including
without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Product, and to permit persons to whom the Product is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions
of the Product.

THE PRODUCT IS PROVIDED ”AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE PRODUCT OR THE USE OR OTHER DEALINGS
IN THE PRODUCT.

## Introduction

The following chapters and sections describe in detail USB Type-C in general and in context of enclustra PE3 board. The USB Type-C controller used on the PE3 is the [ti TPS65987](https://www.ti.com/lit/ds/symlink/tps65987d.pdf).

## Table of contents

* [Chapter 1 - USB Type - C Introduction](./Chapter-1_USB_Type_C_Introduction.md)
* [Chapter 2 - TPS6598x Example Config](./Chapter-2-TPS_Example_Config.md)
* [Chapter 3 - GPIOs](./Chapter-3-GPIOs.md)
* [Chapter 4 - USBCConfig tool](./Chapter-4-USBCConfig_Tool.md)

## References
* [TPS65987D+Docs](https://www.ti.com/product/TPS65987D#tech-docs)
* [TPS65987D+Datasheet](https://www.ti.com/lit/ds/symlink/tps65987d.pdf)
* [TPS65987D+TRM](https://www.ti.com/lit/ug/slvubh2b/slvubh2b.pdf) (Register Description)
* [TPS65987D+GPIO+Events](https://www.ti.com/lit/an/slvae11a/slvae11a.pdf)
* [TPS65987D+I2C+Master](https://www.ti.com/lit/an/slvae18/slvae18.pdf)
* [TPS659xx+Application](https://www.ti.com/lit/ug/slvub60c/slvub60c.pdf)
* [TPS659xx+Batterie+Charging](https://www.ti.com/lit/an/slvae17a/slvae17a.pdf)
* [Displayport+Alt+Mode+Anandtech](https://www.anandtech.com/show/15752/displayport-alt-mode-20-spec-released)
* [Displayport+Alt+Mode+Wiki](https://en.wikipedia.org/wiki/USB-C#Alternate_Mode)
* [TPS65987+Evaluation Module](https://www.ti.com/tool/TPS65987EVM)
* [TPS65988+Evaluation Module](https://www.ti.com/lit/ug/slvub62a/slvub62a.pdf)
