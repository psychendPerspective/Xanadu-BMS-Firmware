# Xanadu-BMS-Firmware
Firmware for Xanadu BMS : This is the source code for the Xanadu BMS.

# Setup Prerequisites

### Windows
- Tools: `git`, `make`, `gcc arm none-eabi`
- Install git : https://git-scm.com/download/win. Make sure to click any boxes to add Git to your Environment (aka PATH)
- Install GNU Arm Embedded Toolchain, version 11.2 2022.02 for Windows : https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
- Install Cygwin : https://www.cygwin.com/ . Ensure `make` package is installed.
- Install OpenOCD : https://github.com/xpack-dev-tools/openocd-xpack/releases
- Install ST-Link Utility : https://www.st.com/en/development-tools/stsw-link004.html


# Build 

### Windows
1. Open a Git terminal in the desired directory by selecting `Git Bash Here` 
2. Type `git clone https://github.com/psychendPerspective/Xanadu-BMS-Firmware.git`
3. Change directory to the recently cloned repository 
4. Type `git checkout origin/main`

5. Now open a Cygwin64 Terminal and change the working directory to the recently cloned repository : `cd /cygdrive/c/...../Xanadu-BMS-Firmware/build_all`
Replace the ..... with your working directory path.
6. Run the build script : `./rebuild_HV.sh`

## Upload Firmware BIN file to the BMS
### Flash it using an ST-Link v2 SWD Debugger

- Flashing to a new board without a bootloader : 
1.  Open STM32 ST-Link Utility, select the address as `0x08032000` and then connect to the Xanadu-BMS with the ST-Link.
2.  Open the bootloader file present in the build_all folder : `generic_bootloader.bin` and flash the bootloader.
3.  Disconnect, select the address as `0x08000000` and then reconnect to the Xanadu-BMS with the ST-Link.
4.  Open the recently built firmware bin file in the build_all/ XANADU_HV_EV folder and flash the firmware.

- Flashing firmware to the board : 
1.  Open STM32 ST-Link Utility.
3.  Select the address as `0x08000000` and then connect to the Xanadu-BMS with the ST-Link.
4.  Open the recently built firmware bin file in the build_all/ XANADU_HV_EV folder and flash the firmware.


# Configuration

### Hardware Configuration
- In generalDefines.h under /Main folder, set XANADU_HV_EV macro as 1
- In 

### Voltage Limits Configuration
- 

### Current Limits Configuration
- 

### Temperature Limits Configuration
- 

# Project Directory


### Flash Memory Management
When flashing the application the start address should be: 0x08000000
When flashing the bootloader the start address should be: 0x08032000

The flash is formatted as follows (summary):

((uint32_t)0x08000000) /* Base @ of Page 0, 2 Kbytes / // Startup Code - Main application
((uint32_t)0x08000800) / Base @ of Page 1, 2 Kbytes / // Page0 - EEPROM emulation
((uint32_t)0x08001000) / Base @ of Page 2, 2 Kbytes / // Page1 - EEPROM emulation
((uint32_t)0x08001800) / Base @ of Page 3, 2 Kbytes / // Remainder of the main application firmware stars from here.
((uint32_t)0x08019000) / Base @ of Page 50, 2 Kbytes / // New app firmware base addres
((uint32_t)0x08032000) / Base @ of Page 100, 2 Kbytes */ // Bootloader base

See "modFlash.h" and "modFlash.c" for more info.