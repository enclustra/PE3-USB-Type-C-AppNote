# Chapter 4 - USB C Config Tool

In this section a short summary about the configtool *USBCConfig* is given.

## 4.1 About

What is the tool *USBCConfig* doing?\
The USB C Config Tool is used to write a configuration file inside the flash memory of the USB C controller chip. The tool uses I2C for this operation.

To generate the configuration file the Application Customization Tool is used as mentioned in chapter 2 of this AppNote. For programming the device, the binary output has to be generated in the export dialog of the Application Customization Tool. For Generation, please follow the instructions described in the TPS Application Customization Tool User Guide.

## 4.2 Sources

The software sources for the tool can be found inside the folder *code* within this AppNote. It can be build by crosscompiling to the desired system with the provided makefile. The output of this build is the tool itself called *USBCConfig*.

## 4.3 Use the USBCConfig

The tool needs two arguments, when it's called. The first one is to define, what action should be executed and the second one is the i2c device, which should be used.

### 4.3.1 Available Actions

One of the following actions can be used:

```c
    "getFWVersion"
    "ProgramRAM"
    "ClearFlash"
    "ProgramFlash"
    "VerifyPatch
```

If no or an unknown action is given, an help dialog will be printed on the terminal.

### 4.3.2 Select I2C device

The selected i2c device has to be the second argument. It is formatted as follows:

- /dev/i2c-0
- /dev/i2c-1
- ...

### 4.3.3 Examples

#### 4.3.3.1 getFwVersion

`.\USBCConfig getfwversion /dev/ic2-1`\
The example above assumes, that the USBC Controller Chip is connected to i2c-1. The command *getfwversion* (not case sensitiv) get's the value for firmware version out of the right register.

#### 4.3.3.2 ProgramFlash

`.\USBCConfig ProgramFlash /dev/ic2-0`\
The example above assumes, that the USBC Controller Chip is connected to i2c-0. The command *ProgramFlash* (not case sensitiv) reads the binary configuration file and writes it in the chips flash. Bevore writing configurations to flash, the RAM has to be updated, too. For this please check if FW Version correspond to FW Version in ti tool.

Hint: For programming the flash, the file has to be named `full_image.bin`. For programming the RAM, the file has to be named `low_region.bin`\
Hint: Bevore flashing the file, a clearflash command is automatically executed.

#### 4.3.3.3 clearFlash

`.\USBCConfig clearflash /dev/ic2-0`\
The example above assumes, that the USBC Controller Chip is connected to i2c-0. The command *clearflash* (not case sensitiv) clears the chips flash.

#### 4.3.3.4 programRAM

`.\USBCConfig programRAM /dev/ic2-0`\
The example above assumes, that the USBC Controller Chip is connected to i2c-0. The command *programRAM* (not case sensitiv) programs the RAM inside the USB C controller chip. The file, which is programmed has to be named `low_region.bin`. The low regions can be exported in the Application customization tool as well as the full image like in the example above.
