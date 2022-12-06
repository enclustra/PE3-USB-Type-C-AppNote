# Chapter 3 - GPIOs

In this section the configured GPIOs of the device are described.

The configuration is done in register 0x5c as described in section 2.2.14 of chapter 2. 


## 3.1 I2C3

For the third I2C on the device the following GPIO pins are used:

- GPIO Pin 5
- GPIO Pin 6

## 3.2 DisplayPort

 For the DP Hotplug detection, there is an option to route the signal to other devices. In this case GPIO 3 is used.

## 3.3 Overview of all user I/Os

| Signal | Name | Description |
| ------ | ------ | ------ |
| GPIO 0  | VCC_3V3_USBCC | - |
| GPIO 1  | GND | - |
| GPIO 2  | Testpoint Z800 | Testpoint on PCB |
| GPIO 3  | USBCC_DP_HPD | Display Port Hot-Plug-Detection signal |
| GPIO 4*  | USBCC_SINK_EN# | Sink enable configuration - config DIP switch "CFG B [4]" |
| GPIO 5  | I2C_SSMUX_SCL | I2C3 SCL signal routed to module |
| GPIO 6  | I2C_SSMUX_SDA | I2C3 SDA signal routed to module |
| GPIO 7  | Testpoint Z801 | Testpoint on PCB |
| GPIO 8  | USBCC_SPI_MISO | SPI MISO signal  |
| GPIO 9  | USBCC_SPI_MOSI | SPI MOSI signal |
| GPIO 10 | USBCC_SPI_CLK | SPI CLK signal |
| GPIO 11 | USBCC_SPI_CS# | SPI chip select signal |
| GPIO 12 | USBCC_GPIO12 | DIP# -> DIP switch "CFG C [1]" |
| GPIO 13 | USBCC_GPIO13 | BTN# -> User button S2103 |
| GPIO 14 | USBCC_GPIO14 | DIP switch "CFG C [3]" SI5332 Input 2 |
| GPIO 15 | USBCC_GPIO15 | DIP switch "CFG C [4]" Connected to System Controller|
| GPIO 16* | PWR_USBC_SRC_EN | Active if power enabled |
| GPIO 17* | PWR_USBC_SINK_EN | Active if power enabled |
| GPIO 18 | USBC_UFP | VBUS ID |
| GPIO 19 | Testpoint Z802 | Testpoint on PCB |
| GPIO 20 | PWR_GOOD | 3.3V Power good indicator |
| GPIO 21 | USBCC_LED# | Yellow USB Type-C indicator LED |

\* GPIO4 - If the input is logic low, it acts as a power sink with 5V/3A up to 9V/3A ("PWR_USBCC_SINK_EN"). And if the input is logic high, it should not be a sink. \
\* GPIO16/17 - Active, depending on the supplied voltage. The event "Port 0 SourceSinkEvent" can be used.

**The next chapter of this application note is [Chapter 4 - USBCConfig Tool](./Chapter-4-USBCConfig_Tool.md).**
