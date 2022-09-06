
/*
	Copyright 2017 - 2018 Danny Bokma	danny@diebie.nl
	Copyright 2019 - 2020 Kevin Dionne	kevin.dionne@ennoid.me
  	Copyright 2022        Vishal Bhat vishal.bhat09@gmail.com

	This file is part of the Xanadu BMS firmware.

	The Xanadu BMS firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The Xanadu BMS firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "driverHWSPI1.h"
#include "stdlib.h"
#include "math.h"
#include "mainDataTypes.h"
#include "modDelay.h"
#include "generalDefines.h"


/*
	Pre computed crc15 table used for the LTC6804 PEC calculation

	The code used to generate the crc15 table is:

void generate_crc15_table()
{
  int remainder;
	for(int i = 0; i<256;i++)
	{
		remainder =  i<< 7;
		for (int bit = 8; bit > 0; --bit)
  		  {

     			 if ((remainder & 0x4000) > 0)//equivalent to remainder & 2^14 simply check for MSB
    			  {
        				remainder = ((remainder << 1)) ;
        				remainder = (remainder ^ 0x4599);
     			 }
     			 else
      			{
       				 remainder = ((remainder << 1));
      			}
   		 }

		crc15Table[i] = remainder&0xFFFF;
	}
}
*/
static const unsigned int crc15Table[256] = {
	  0x0000, 0xc599, 0xceab, 0x0b32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,  //!<precomputed CRC15 Table
		0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,
		0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
		0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
		0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,
		0x2544, 0x02be, 0xc727, 0xcc15, 0x098c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c,
		0x3d6e, 0xf8f7, 0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x07c2, 0xc25b, 0xc969, 0x0cf0, 0xdf0d,
		0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
		0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
		0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
		0x4a88, 0x8f11, 0x057c, 0xc0e5, 0xcbd7, 0x0e4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,
		0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921,
		0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070,
		0x85e9, 0x0f84, 0xca1d, 0xc12f, 0x04b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a,
		0x3528, 0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2,
		0xe46b, 0xef59, 0x2ac0, 0x0d3a, 0xc8a3, 0xc391, 0x0608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7,
		0x54aa, 0x9133, 0x9a01, 0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06,
		0xab9f, 0x7862, 0xbdfb, 0xb6c9, 0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80,
		0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a, 0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41,
		0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25, 0x2fbc, 0x0846, 0xcddf, 0xc6ed, 0x0374,
		0xd089, 0x1510, 0x1e22, 0xdbbb, 0x0af8, 0xcf61, 0xc453, 0x01ca, 0xd237, 0x17ae, 0x1c9c,
		0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b, 0x2d02, 0xa76f, 0x62f6,
		0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3, 0x585a, 0x8ba7,
    0x4e3e, 0x450c, 0x8095
};

 /*!

  |MD| Dec  | ADC Conversion Model|
  |--|------|---------------------|
  |01| 1    | Fast 			   	  		|
  |10| 2    | Normal 			 				|
  |11| 3    | Filtered 		   	  	|
*/
#define MD_FAST 1
#define MD_NORMAL 2
#define MD_FILTERED 3


 /*!
 |CH | Dec  | Channels to convert |
 |---|------|---------------------|
 |000| 0    | All Cells  		  |
 |001| 1    | Cell 1 and Cell 7   |
 |010| 2    | Cell 2 and Cell 8   |
 |011| 3    | Cell 3 and Cell 9   |
 |100| 4    | Cell 4 and Cell 10  |
 |101| 5    | Cell 5 and Cell 11  |
 |110| 6    | Cell 6 and Cell 12  |
*/

#define CELL_CH_ALL 0
#define CELL_CH_1and7 1
#define CELL_CH_2and8 2
#define CELL_CH_3and9 3
#define CELL_CH_4and10 4
#define CELL_CH_5and11 5
#define CELL_CH_6and12 6



/*!

  |CHG | Dec  |Channels to convert   |
  |----|------|----------------------|
  |000 | 0    | All GPIOS and 2nd Ref|
  |001 | 1    | GPIO 1 			    		 |
  |010 | 2    | GPIO 2               |
  |011 | 3    | GPIO 3 			  			 |
  |100 | 4    | GPIO 4 			  			 |
  |101 | 5    | GPIO 5 			 				 |
  |110 | 6    | Vref2  			  			 |
*/

#define AUX_CH_ALL 0
#define AUX_CH_GPIO1 1
#define AUX_CH_GPIO2 2
#define AUX_CH_GPIO3 3
#define AUX_CH_GPIO4 4
#define AUX_CH_GPIO5 5
#define AUX_CH_VREF2 6

//uint8_t CHG = 0; //!< aux channels to be converted
 /*!****************************************************
  \brief Controls if Discharging transitors are enabled
  or disabled during Cell conversions.

 |DCP | Discharge Permitted During conversion |
 |----|----------------------------------------|
 |0   | No - discharge is not permitted         |
 |1   | Yes - discharge is permitted           |

********************************************************/
#define DCP_DISABLED 0
#define DCP_ENABLED 1


#define CELL 1
#define AUX 2
#define STAT 3
#define CFGR 0
#define CFGRB 4

typedef enum {
	LTC6804WriteConfigRegA                             = 0x0001, // WRCFG
	LTC6804WriteConfigRegB                             = 0x0024, // WRCFG
	LTC6804ReadConfigRegA                              = 0x0002, // RDCFG
	LTC6804ReadConfigRegB                              = 0x0026, // RDCFG
	LTC6804ReadCellVoltageGroupA                   = 0x0004, // RDVCA
	LTC6804ReadCellVoltageGroupB                   = 0x0006, // RDVCB
	LTC6804ReadCellVoltageGroupC                   = 0x0008, // RDVCC
	LTC6804ReadCellVoltageGroupD                   = 0x000A, // RDVCD
	LTC6804ReadAuxGroupA                           = 0x000C, // RDAUXA
	LTC6804ReadAuxGroupB                           = 0x000E, // RDAUXB
	LTC6804ReadStatusRegisterGroupA                = 0x0010, // RDSTATA
	LTC6804ReadStatusRegisterGroupB                = 0x0012, // RDSTATB
	LTC6804StartCellVoltageADConversion            = 0x0260, // ADCV
	LTC6804StartCellOpenWireVoltageADConversion    = 0x0228, // ADOW
	LTC6804StartCellSelfTestVoltageConversion      = 0x0207, // CVST
	LTC6804StartGPIOADConversion                   = 0x0460, // ADAX
	LTC6804SelfTestGPIOConversion                  = 0x0407, // AXST
	LTC6804StartStatusGroupConversion              = 0x0468, // ADSTAT
	LTC6804StartSelfTestStatusGroup                = 0x040F, // STATST
	LTC6804StartCombinedADConversion               = 0x046F, // ADCVAX
	LTC6804ClearCellVoltageGroup                   = 0x0711, // CLRCELL
	LTC6804ClearAuxVoltageGroup                    = 0x0712, // CLRAUX
	LTC6804ClearStatusRegisterGroup                = 0x0713, // CLRSTAT
	LTC6804PollADConversion                        = 0x0714, // PLADC
	LTC6804DiagnoseMUX                             = 0x0715, // DIAGN
	LTC6804CommunicationWriteRegisterGroup         = 0x0721, // WRCOMM
	LTC6804CommunucationReadRegisterGroup          = 0x0722, // RDCOMM
	LTC6804CommuncationStart                       = 0x0723  // STCOMM
} driverSWLTC6804RegistersBase;

typedef struct {
	bool GPIO1;																																							// Read/Write opendrain GPIO1
	bool GPIO2;																																							// Read/Write opendrain GPIO2
	bool GPIO3;																																							// Read/Write opendrain GPIO3
	bool GPIO4;																																							// Read/Write opendrain GPIO4
	bool GPIO5;																																							// Read/Write opendrain GPIO5
	bool GPIO6;																																							// Read/Write opendrain GPIO6
	bool GPIO7;																																							// Read/Write opendrain GPIO7
	bool GPIO8;																																							// Read/Write opendrain GPIO8
	bool GPIO9;																																							// Read/Write opendrain GPIO9
	bool ReferenceON;																																			  // Reference ON
	bool SoftwareTimerFlag;																																	// Read software timer pin
	bool ADCOption;																																				  // ADC Option register for configuration of over sampling ratio
	uint8_t noOfCells;																																			// Number of cells to monitor (that can cause interrupt)
	uint32_t DisChargeEnableMask;																														// Set enable state of discharge, 1=EnableDischarge, 0=DisableDischarge
	uint8_t DischargeTimout;																																// Discharge timout value / limit
	float CellUnderVoltageLimit;																														// Undervoltage level, cell voltages under this limit will cause interrupt
	float CellOverVoltageLimit;																															// Over voltage limit, cell voltages over this limit will cause interrupt
} driverLTC6804ConfigStructTypedef;

typedef struct {
  float sumOfCells;
	float dieTemperature;
	float voltageAnalogSupply;
	float voltageDigitalSupply;
	uint16_t overVoltage;
	uint16_t underVoltage;
	bool  muxFail;
} driverSWLTC6804StatusStructTypedef;

/*! Cell Voltage data structure. */
typedef struct
{
  uint16_t c_codes[18]; //!< Cell Voltage Codes
  uint8_t pec_match[6]; //!< If a PEC error was detected during most recent read cmd
} cv;

/*! AUX Reg Voltage Data structure */
typedef struct
{
  uint16_t a_codes[9]; //!< Aux Voltage Codes
  uint8_t pec_match[4]; //!< If a PEC error was detected during most recent read cmd
} ax;

/*! Status Reg data structure. */
typedef struct
{
  uint16_t stat_codes[4]; //!< Status codes.
  uint8_t flags[3]; //!< Byte array that contains the uv/ov flag data
  uint8_t mux_fail[1]; //!< Mux self test status flag
  uint8_t thsd[1]; //!< Thermal shutdown status
  uint8_t pec_match[2]; //!< If a PEC error was detected during most recent read cmd
} st;

/*! IC register structure. */
typedef struct
{
  uint8_t tx_data[6];  //!< Stores data to be transmitted
  uint8_t rx_data[8];  //!< Stores received data
  uint8_t rx_pec_match; //!< If a PEC error was detected during most recent read cmd
} ic_register;

/*! PEC error counter structure. */
typedef struct
{
  uint16_t pec_count; //!< Overall PEC error count
  uint16_t cfgr_pec;  //!< Configuration register data PEC error count
  uint16_t cell_pec[6]; //!< Cell voltage register data PEC error count
  uint16_t aux_pec[4];  //!< Aux register data PEC error count
  uint16_t stat_pec[2]; //!< Status register data PEC error count
} pec_counter;

/*! Register configuration structure */
typedef struct
{
  uint8_t cell_channels; //!< Number of Cell channels
  uint8_t stat_channels; //!< Number of Stat channels
  uint8_t aux_channels;  //!< Number of Aux channels
  uint8_t num_cv_reg;    //!< Number of Cell voltage register
  uint8_t num_gpio_reg;  //!< Number of Aux register
  uint8_t num_stat_reg;  //!< Number of  Status register
} register_cfg;

typedef struct {
	  ic_register config;
	  ic_register configb;
	  cv  cells;
	  ax  aux;
	  st  stat;
	  ic_register com;
	  ic_register pwm;
	  ic_register pwmb;
	  ic_register sctrl;
	  ic_register sctrlb;
	  uint8_t sid[6];
	  bool isospi_reverse;
	  pec_counter crc_count;
	  register_cfg ic_reg;
	  long system_open_wire;
}cell_asic;

//Init
void     driverSWLTC6804Init(driverLTC6804ConfigStructTypedef configStruct, uint8_t totalNumberOfLTCs, uint8_t maxNoOfCellPerModule, uint8_t maxNoOfTempSensorPerModule, uint8_t cellMonitorType);

//Write/read config registers
void     driverSWLTC6804WriteConfigRegister(uint8_t totalNumberOfLTCs, uint32_t *balanceEnableMaskArray, bool useArray);
void     driverSWLTC6804WriteConfigRegisterB(uint8_t totalNumberOfLTCs, uint32_t *balanceEnableMaskArray, bool useArray);
int8_t   driverSWLTC6804ReadConfigRegister(uint8_t nIC, uint8_t r_config[][8]);

//Balance resistor
void     driverSWLTC6804EnableBalanceResistors(uint32_t enableMask, uint8_t cellMonitorType); // Used only for single slave  test with "testbms" command
void     driverSWLTC6804EnableBalanceResistorsArray(uint32_t *enableMask, uint8_t cellMonitorType);

//Cell voltage
void     driverSWLTC6804StartCellAndAuxVoltageConversion(uint8_t MD,uint8_t DCP);
void     driverSWLTC6804StartCellVoltageConversion(uint8_t MD,uint8_t DCP, uint8_t CH);
void     driverSWLTC6804StartLoadedCellVoltageConversion(uint8_t MD,uint8_t DCP, uint8_t CH,uint8_t PUP);

bool     driverSWLTC6804ReadCellVoltagesArray(float cellVoltagesArray[][18]);
uint8_t  driverSWLTC6804ReadCellVoltageRegisters(uint8_t reg, uint8_t total_ic, uint16_t cell_codes[][18]);
void     driverSWLTC6804ReadCellVoltageGroups(uint8_t reg, uint8_t total_ic, uint8_t *data);

// Aux sensors
void     driverSWLTC6804StartAuxVoltageConversion(uint8_t MD, uint8_t CHG);
float    driverSWLTC6804ConvertTemperatureExt(uint16_t inputValue,uint32_t ntcNominal,uint32_t ntcSeriesResistance,uint16_t ntcBetaFactor, float ntcNominalTemp);

bool     driverSWLTC6804ReadAuxVoltagesArray(float auxVoltagesArray[][9],uint32_t ntcNominal,uint32_t ntcSeriesResistance, uint16_t ntcBetaFactor,float ntcNominalTemp);
int8_t   driverSWLTC6804ReadAuxVoltageRegisters(uint8_t reg, uint8_t total_ic, uint16_t aux_codes[][12]);
void     driverSWLTC6804ReadAuxGroups(uint8_t reg, uint8_t total_ic,uint8_t *data);
bool driverSWLTC6804ReadPackCurrent(float auxVoltagesArray[][12]);
bool driverSWLTC6804ReadVREFvoltage(float auxVoltageVREFArray[][12]);

//Status & flags
bool     driverSWLTC6804ReadVoltageFlags(uint32_t *underVoltageFlags, uint32_t *overVoltageFlags, uint32_t lastICMask, uint8_t noOfParallelModules, uint32_t dieTemperature[]);
uint8_t  driverSWLTC6804ReadStatusValues(uint8_t total_ic, driverSWLTC6804StatusStructTypedef statusArray[]);
void     driverSWLTC6804ReadStatusGroups(uint8_t reg, uint8_t total_ic, uint8_t *data );

//Utilities
void     driverSWLTC6804ResetCellVoltageRegisters(void);
void     driverSWLTC6804ResetAuxRegisters(void);
void     driverSWLTC6804ResetStatusRegisters(void);
void     driverSWLTC6804DelayMS(uint32_t delayMS);
void     driverSWLTC6804Write(uint8_t *writeBytes, uint8_t writeLength);
void     driverSWLTC6804WriteRead(uint8_t *writeBytes, uint8_t writeLength, uint8_t *readBytes, uint8_t readLength);
void     driverSWLTC6804WakeIC(void);
uint16_t driverSWLTC6804CalcPEC15(uint8_t len, uint8_t *data);



void write_68(uint8_t total_ic , //!< Number of ICs in the daisy chain
              uint8_t tx_cmd[2], //!< 2 byte array containing the BMS command to be sent
              uint8_t data[] //!< Array containing the data to be written to the BMS ICs
             );

/*!
 Issues a command onto the daisy chain and reads back 6*total_ic data in the rx_data array
 @return int8_t, PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t read_68( uint8_t total_ic, //!< Number of ICs in the daisy chain
                uint8_t tx_cmd[2], //!< 2 byte array containing the BMS command to be sent
                uint8_t *rx_data); //!< Array that the read back data will be stored in.
/*!
 Helper Function to Set DCC bits in the CFGR Registers
 @return void
 */
void LTC6813_set_discharge(int Cell, //!< The cell to be discharged
                           uint8_t total_ic, //!< Number of ICs in the system
                           cell_asic *ic //!< A two dimensional array that will store the data
						   );

/*!
 Helper Function to clear DCC bits in the CFGR Registers
 @return void
 */
void LTC681x_clear_discharge(uint8_t total_ic,//!< Number of ICs in the daisy chain
                             cell_asic *ic //!< A two dimensional array that will store the data
							 );

/*!
 Write the LTC681x PWM register
 This command will write the pwm registers of the LTC681x connected in a daisy chain stack.
 The pwm is written in descending order so the last device's pwm is written first.
 @return void
 */
void LTC681x_wrpwm(uint8_t total_ic, //!< The number of ICs being written to
                   uint8_t pwmReg,  //!< The PWM Register to be written
                   cell_asic *ic //!< A two dimensional array that will store the data to be written
                  );

/*!
  Reads pwm registers of a LTC681x daisy chain
  @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t LTC681x_rdpwm(uint8_t total_ic, //!< Number of ICs in the system
                     uint8_t pwmReg, //!< The PWM Register to be written A or B
                     cell_asic *ic //!< A two dimensional array that will store the read data
                    );

/*!
 Helper function to set appropriate bits in CFGR register based on bit function
 @return void
 */
void LTC6813_set_cfgr(uint8_t nIC,  //!< The number of ICs in the daisy chain
                      cell_asic *ic, //!< A two dimensional array that will store the data
                      bool refon, //!< The REFON bit
                      bool adcopt, //!< The ADCOPT bit
                      bool gpio[5], //!< The GPIO bits
                      bool dcc[12], //!< The DCC bits
					  bool dcto[4], //!< The Dcto bits
					  uint16_t uv, //!< The UV value
					  uint16_t  ov //!< The OV value
					  );

/*!
 Helper function to turn the REFON bit HIGH or LOW
 @return void
 */
void LTC681x_set_cfgr_refon(uint8_t nIC, //!< Current IC
                            cell_asic *ic, //!< A two dimensional array that will store the data
                            bool refon //!< The REFON bit
							);

/*!
 Helper function to turn the ADCOPT bit HIGH or LOW
 @return void
 */
void LTC681x_set_cfgr_adcopt(uint8_t nIC, //!< Current IC
                             cell_asic *ic, //!< A two dimensional array that will store the data
                             bool adcopt //!< The ADCOPT bit
							 );

/*!
 Helper function to turn the GPIO bits HIGH or LOW
 @return void
 */
void LTC681x_set_cfgr_gpio(uint8_t nIC, //!< Current IC
                           cell_asic *ic, //!< A two dimensional array that will store the data
                           bool gpio[] //!< The GPIO bits
						   );

/*!
 Helper function to turn the DCC bits HIGH or LOW
 @return void
 */
void LTC681x_set_cfgr_dis(uint8_t nIC, //!< Current IC
                          cell_asic *ic, //!< A two dimensional array that will store the data
                          bool dcc[] //!< The DCC bits
						  );

/*!
 Helper function to control discharge time value
 @return void
 */
void LTC681x_set_cfgr_dcto(uint8_t nIC,  //!< Current IC
						   cell_asic *ic, //!< A two dimensional array that will store the data
						   bool dcto[] //!< The Dcto bits
						   );

/*!
 Helper function to set uv field in CFGRA register
 @return void
 */
void LTC681x_set_cfgr_uv(uint8_t nIC, //!< Current IC
                         cell_asic *ic, //!< A two dimensional array that will store the data
                         uint16_t uv //!< The UV value
						 );

/*!
 Helper function to set ov field in CFGRA register
 @return void
 */
void LTC681x_set_cfgr_ov(uint8_t nIC, //!< Current IC
                         cell_asic *ic, //!< A two dimensional array that will store the data
                         uint16_t ov //!< The OV value
						 );

/*!
 Helper function to set appropriate bits in CFGR register based on bit function
 @return void
 */
void LTC6813_set_cfgrb(uint8_t nIC, //!< The number of ICs in the daisy chain
                      cell_asic *ic, //!< A two dimensional array that will store the data
					  bool fdrf, //!< The FDRF bit
                      bool dtmen, //!< The DTMEN bit
                      bool ps[2], //!< Path selection bits
                      bool gpiobits[4], //!< The GPIO bits
					  bool dccbits[7] //!< The DCC bits
					  );

/*!
 Helper function to turn the FDRF bit HIGH or LOW
 @return void
 */
void LTC6813_set_cfgrb_fdrf(uint8_t nIC, //!< The number of ICs in the daisy chain
                            cell_asic *ic, //!< A two dimensional array that will store the data
                            bool fdrf //!< The FDRF bit
							);

/*!
 Helper function to turn the DTMEN bit HIGH or LOW
 @return void
 */
void LTC6813_set_cfgrb_dtmen(uint8_t nIC, //!< The number of ICs in the daisy chain
                            cell_asic *ic, //!< A two dimensional array that will store the data
                            bool dtmen //!< The DTMEN bit
							);

/*!
 Helper function to turn the Path Select bit HIGH or LOW
 @return void
 */
void LTC6813_set_cfgrb_ps(uint8_t nIC, //!< The number of ICs in the daisy chain
                            cell_asic *ic, //!< A two dimensional array that will store the data
                            bool ps[] //!< Path selection bits
							);

/*!
 Helper function to turn the GPIO bit HIGH or LOW
 @return void
 */
void LTC6813_set_cfgrb_gpio_b(uint8_t nIC, //!< The number of ICs in the daisy chain
                            cell_asic *ic, //!< A two dimensional array that will store the data
                            bool gpiobits[] //!< The GPIO bits
							);

/*!
 Helper function to turn the DCC bit HIGH or LOW
 @return void
 */
void LTC6813_set_cfgrb_dcc_b(uint8_t nIC, //!< The number of ICs in The daisy chain
                            cell_asic *ic, //!< A two dimensional array that will store The data
                            bool dccbits[] //!< The DCC bits
							);

/*!
 Write the LTC681x CFGRA register
 This command will write the configuration registers of the LTC681xs connected in a daisy chain stack.
 The configuration is written in descending order so the last device's configuration is written first.
 @return void
 */
void LTC681x_wrcfg(uint8_t total_ic, //!< The number of ICs being written to
                   cell_asic *ic //!< A two dimensional array of the configuration data that will be written
                  );

/*!
 Write the LTC681x CFGRB register
 This command will write the configuration registers of the LTC681xs connected in a daisy chain stack.
 The configuration is written in descending order so the last device's configuration is written first.
 @return void
 */
void LTC681x_wrcfgb(uint8_t total_ic, //!< The number of ICs being written to
                    cell_asic *ic //!< A two dimensional array of the configuration data that will be written
                   );

/*!
 Reads the LTC681x CFGRA register
 @return int8_t, PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t LTC681x_rdcfg(uint8_t total_ic, //!< Number of ICs in the system
                     cell_asic *ic //!< A two dimensional array that the function stores the read configuration data.
                    );

/*!
 Reads the LTC681x CFGRB register
 @return int8_t, PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t LTC681x_rdcfgb(uint8_t total_ic, //!< Number of ICs in the system
                      cell_asic *ic //!< A two dimensional array that the function stores the read configuration data.
                     );

/*!
Helper Function that counts overall PEC errors and register/IC PEC errors
@return void
*/
void LTC681x_check_pec(uint8_t total_ic, //!< Number of ICs in the daisy chain
                      uint8_t reg, //!< Type of register
                      cell_asic *ic //!< A two dimensional array that will store the data
					   );

