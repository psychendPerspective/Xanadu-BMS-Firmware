# Xanadu-BMS-Firmware
Firmware for Xanadu BMS : This is the source code for the Xanadu BMS, based on the Ennoid-BMS

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
5. Now open a Cygwin64 Terminal and change the working directory to the recently cloned repository : <br />
`cd /cygdrive/c/...../Xanadu-BMS-Firmware/build_all`
Replace the ..... with your working directory path.<br />
6. Run the build script : `./rebuild_HV.sh`

### Upload Firmware BIN file to the BMS
#### Flash the bin file using an ST-Link v2 SWD Debugger

- Flashing to a new board without a bootloader : 
1.  Open STM32 ST-Link Utility, select the address as `0x08032000` and then connect to the Xanadu-BMS with the ST-Link.
2.  Open the bootloader file present in the build_all folder : `generic_bootloader.bin` and flash the bootloader.
3.  Disconnect, select the address as `0x08000000` and then reconnect to the Xanadu-BMS with the ST-Link.
4.  Open the recently built firmware bin file in the `build_all/ XANADU_HV_EV` folder and flash the firmware.

- Flashing firmware to the board : 
1.  Open STM32 ST-Link Utility.
3.  Select the address as `0x08000000` and then connect to the Xanadu-BMS with the ST-Link.
4.  Open the recently built firmware bin file in the `build_all/ XANADU_HV_EV` folder and flash the firmware.


# Configuration for Xanadu-BMS

### Hardware Configuration
- In `generalDefines.h` in the `Main` folder, set `XANADU_HV_EV` macro as 1
- Set the other macros under `XANADU_HV_EV` based on the hardware configuration
- In `modConfig.c` in the `Modules/Src` folder, under `modConfigLoadDefaultConfig`, configure the following based on the 
battery pack : <br />
1. `noOfCellsSeries` : Total number of cells in series in the battery pack
2. `noOfCellsParallel` : Number of cells in parallel per series string in the battery pack
3. `batteryCapacity` : Battery Pack Capacity in Ah
4. `CANID` : Set the CAN ID for the BMS (1-255)
5. `canBusSpeed` :  Set the Baud rate for CAN transmission
6. `noOfTempSensorPerModule` : Total number of thermistors present which are being monitored per LTC681x AFE IC
7. `cellMonitorType` : Set the type of AFE Cell Monitor IC used (LTC6811, LTC6812, LTC6813)
8. `cellMonitorICCount` : Total number of AFE Cell Monitor ICs connected in the stack
9. `noOfCellsPerModule` : Total number of cells being monitored per AFE Cell Monitor IC
10. `lastICNoOfCells` : Number of cells being monitored by the last AFE Cell Monitor IC in the stack
11. `cellTypeUsed` : Select the cell Type used in the battery pack to calculate initial SoC


### Parameter Configuration
- In `modConfig.c` in the `Modules/Src` folder, under `modConfigLoadDefaultConfig`, configure the following based on the 
battery pack : <br />
#### UnderVoltage Limits
1. `cellHardUnderVoltage` : Absolute minimum voltage(V) for the lowest cell voltage
2. `cellLCSoftUnderVoltage` : Lower Cell voltage limit for the BMS to cut-off during operation

#### OverVoltage Limits
1. `cellHardOverVoltage`: Absolute maximum voltage(V) for the highest cell voltage
2. `cellSoftOverVoltage` : Upper Cell voltage limit for the BMS to cut-off during operation

#### Current Limits Configuration
- Note : Positive Current is taken as Charging Current while Negative Current is taken as Discharging Current
1. `maxAllowedCurrent` : Absolute maximum allowed current(A) through the BMS without triggering an error
2. `maxAllowedChargingCurrent` : Current Limit for the BMS to cut-off during Charging. Set this limit based on the maximum recommended charging current for the cells used in the battery pack.
3. `maxAllowedDischargingCurrent` : Current Limit for the BMS to cut-off during Discharging . Set this limit based on the load rating/ maximum recommended discharging current for the cells used in the battery pack. This value should have a negative sign.

#### Temperature Limits Configuration
1. `allowedTempBattDischargingMax` : Maximum Battery Pack Temperature(degC) allowed during Discharging
2. `allowedTempBattDischargingMin` : Minimum Battery Pack Temperature (degC)allowed during Discharging
3. `allowedTempBattChargingMax` : Maximum Battery Pack Temperature(degC) allowed during Charging
4. `allowedTempBattChargingMin` : Minimum Battery Pack Temperature(degC) allowed during Charging
5. `allowedTempBMSMax` : Absolute Maximum temperature(degC) allowed for normal BMS operation
6. `allowedTempBMSMin` : Absolute Minimum temperature(degC) allowed for normal BMS operation

#### Passive Cell Balancing Configuration
1. `cellBalanceStart` : Start balancing above this cell voltage(V) during charging
2. `cellBalanceDifferenceThreshold` : If the Cell Imbalance Voltage, i.e (Cell Voltage - Lowest Cell Voltage) is above this voltage limit, cell balancing will occur 
3. `cellBalanceAllTime` : Enable cell balancing under all operational states

#### Logging Interval 
- In `modSDcard.h` in the `Modules/Inc` folder,configure the following : <br />
1. `LOGGING_INTERVAL` : Set the SD card logging interval in milliseconds.

# Project Directory Layout
- `Main` folder contains the main application code
- `Modules` folder contains the source and header files for the application layer
- `Libraries` folder contains the source and header files for the external libraries used
- `Drivers` folder contains drivers for the hardware and software peripherals, as well as the STM32 HAL drivers
- `GCC` folder contains the startup code and linker script required for the controller
- `build_all` folder contains the build scripts and firmware build files

#### Operational and Fault States

- The integer values present in the SD card logging file correspond to the following operational and fault state enums respectively, present in `Main/mainDataTypes.h`

```
//Operational States
typedef enum {
	OP_STATE_INIT = 0,		     // 0
	OP_STATE_CHARGING,		     // 1
	OP_STATE_PRE_CHARGE,		 // 2
	OP_STATE_LOAD_ENABLED,		 // 3
	OP_STATE_BATTERY_DEAD,		 // 4
	OP_STATE_POWER_DOWN,		 // 5
	OP_STATE_EXTERNAL,		     // 6
	OP_STATE_ERROR,			     // 7
	OP_STATE_ERROR_PRECHARGE,	 // 8
	OP_STATE_BALANCING,		     // 9
	OP_STATE_CHARGED,		     // 10
	OP_STATE_FORCEON,	 	     // 11
} OperationalStateTypedef;

//Fault States
typedef enum {
	FAULT_CODE_NONE = 0,                    //0
	FAULT_CODE_PACK_OVER_VOLTAGE,           //1
	FAULT_CODE_PACK_UNDER_VOLTAGE,          //2
	FAULT_CODE_LOAD_OVER_VOLTAGE,           //3
	FAULT_CODE_LOAD_UNDER_VOLTAGE,          //4
	FAULT_CODE_CHARGER_OVER_VOLTAGE,        //5
	FAULT_CODE_CHARGER_UNDER_VOLTAGE,       //6
	FAULT_CODE_CELL_HARD_OVER_VOLTAGE,      //7
	FAULT_CODE_CELL_HARD_UNDER_VOLTAGE,     //8
	FAULT_CODE_CELL_SOFT_OVER_VOLTAGE,      //9
	FAULT_CODE_CELL_SOFT_UNDER_VOLTAGE,     //10
	FAULT_CODE_MAX_UVP_OVP_ERRORS,          //11
	FAULT_CODE_MAX_UVT_OVT_ERRORS,          //12
	FAULT_CODE_OVER_CURRENT,                //13
	FAULT_CODE_OVER_TEMP_BMS,               //14
	FAULT_CODE_UNDER_TEMP_BMS,              //15
	FAULT_CODE_DISCHARGE_OVER_TEMP_CELLS,   //16
	FAULT_CODE_DISCHARGE_UNDER_TEMP_CELLS,  //17
	FAULT_CODE_CHARGE_OVER_TEMP_CELLS,      //18
	FAULT_CODE_CHARGE_UNDER_TEMP_CELLS,     //19
	FAULT_CODE_PRECHARGE_TIMEOUT,           //20
	FAULT_CODE_DISCHARGE_RETRY,             //21
	FAULT_CODE_CHARGE_RETRY,                //22
	FAULT_CODE_CAN_DELAYED_POWER_DOWN,      //23
	FAULT_CODE_NOT_USED_TIMEOUT,            //24
	FAULT_CODE_CHARGER_DISCONNECT,          //25
	FAULT_CODE_MAX_SOFT_UVP_ERRORS          //26
} bms_fault_state;


```

#### Flash Memory Management
When flashing the application the start address should be: `0x08000000`
When flashing the bootloader the start address should be: `0x08032000`

The flash is formatted as follows (summary):

- ((uint32_t)0x08000000) :  Base @ of Page 0, 2 Kbytes   // Startup Code - Main application
- ((uint32_t)0x08000800) :  Base @ of Page 1, 2 Kbytes   // Page0 - EEPROM emulation
- ((uint32_t)0x08001000) :  Base @ of Page 2, 2 Kbytes   // Page1 - EEPROM emulation
- ((uint32_t)0x08001800) :  Base @ of Page 3, 2 Kbytes   // Remainder of the main application firmware stars from here.
- ((uint32_t)0x08019000) :  Base @ of Page 50, 2 Kbytes  // New app firmware base addres
- ((uint32_t)0x08032000) :  Base @ of Page 100, 2 Kbytes // Bootloader base

See `modFlash.h` and `modFlash.c` for more info.
