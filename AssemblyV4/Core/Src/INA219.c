/*
 * INA219.c
 *
 *  Created on: Nov 2, 2020
 *      Author: marci
 */
#include "main.h"
#include "INA219.h"
#include "i2c.h"


void ina_init (void) {


    ina219_currentDivider_mA = 0;
    ina219_powerMultiplier_mW = 0;

    // Option 1: setCalibration_32V_2A();
    // Option 2: setCalibration_32V_1A();
  // ina_calib_32V_1A(); // Option 3
    ina_calib_16V_200mA();
}

void ina_write(uint8_t reg, uint16_t value)
{
	  uint8_t val8b[2];



	  val8b[1] = (uint8_t) (value & 0xFF);
	  val8b[0] = (uint8_t) ((value >> 8) & 0xFF);


	HAL_I2C_Mem_Write(&hi2c1, INA_219_ADDR , reg, 1, val8b, sizeof(val8b), HAL_MAX_DELAY);

}

//Function which gives two bytes data info to *value
void ina_read(uint8_t reg, uint16_t *value)
{
	HAL_StatusTypeDef ret1;
	uint8_t buf[2];
	ret1 = HAL_I2C_Mem_Read(&hi2c1, INA_219_ADDR, reg, 1, buf, 2, HAL_MAX_DELAY);

	*value = ((uint16_t)buf[0]<<8 | (uint8_t)buf[1]);



}


void ina_calib_32V_1A (void) {
    ina219_calibrationValue = 10240;

    ina219_currentDivider_mA = 25;
    ina219_powerMultiplier_mW = 1;

    ina_write(INA219_REG_CALIBRATION, ina219_calibrationValue);

    ina_write(INA219_REG_CONFIG, (INA219_CONFIG_BVOLTAGERANGE_32V |
                    INA219_CONFIG_GAIN_8_320MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS));
}

void ina_calib_16V_400mA (void) {
    ina219_calibrationValue = 8192;

    ina219_currentDivider_mA = 20;
    ina219_powerMultiplier_mW = 1;
    uint16_t cali = 8192;

    ina_write(INA219_REG_CALIBRATION, ina219_calibrationValue<<1);

    ina_write(INA219_REG_CONFIG, (INA219_CONFIG_BVOLTAGERANGE_16V |
                    INA219_CONFIG_GAIN_1_40MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS));
}

void ina_calib_16V_200mA (void) {
    ina219_calibrationValue = 40960;

    ina219_currentDivider_mA = 20;
    ina219_powerMultiplier_mW = 1;

    ina_write(INA219_REG_CALIBRATION, ina219_calibrationValue);

    ina_write(INA219_REG_CONFIG, (INA219_CONFIG_BVOLTAGERANGE_16V |
                    INA219_CONFIG_GAIN_1_40MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS));

}


uint16_t getBusVoltage_raw (void) {

	uint16_t value;
	uint8_t buf[2];

	/*HAL_I2C_Master_Transmit(&hi2c1, INA_219_ADDR, INA219_REG_BUSVOLTAGE, 1, HAL_MAX_DELAY);
	//ina_read(INA219_REG_BUSVOLTAGE, &value);

	if (HAL_I2C_Master_Receive(&hi2c1, INA_219_ADDR, &buf, 2, HAL_MAX_DELAY)==HAL_OK);
	value = ((uint16_t)buf[0]<<8 | (uint8_t)buf[1]);
	*/

	HAL_I2C_Mem_Read(&hi2c1, INA_219_ADDR, INA219_REG_BUSVOLTAGE, 1, buf, 2, 100);
	HAL_Delay(500);


	value = (((uint16_t)buf[0]<<8 | (uint8_t)buf[1])>>3)*4;

	return value;
	//return (uint16_t)((value >> 3)*4);
}


uint16_t getShuntVoltage_raw (void) {
	uint16_t value;

	ina_read(INA219_REG_SHUNTVOLTAGE, &value);

	return (uint16_t) value;
}

uint16_t getCurrent_raw (void) {
	uint16_t value;

	ina_write(INA219_REG_CALIBRATION, ina219_calibrationValue);

	ina_read(INA219_REG_CURRENT, &value);

	return (uint16_t) value;
}

uint16_t getPower_raw (void) {
    uint16_t value;

    //ina_write(INA219_REG_CALIBRATION, ina219_calibrationValue);

    ina_read(INA219_REG_POWER, &value);

    return (uint16_t) value;
}

float getBusVoltage_V (void) {
	uint16_t value = getBusVoltage_raw();
	return value * 0.001;
}

float getShuntVoltage_mV (void) {
	uint16_t value = getShuntVoltage_raw();

	return value * 0.01;
}

float getCurrent_mA (void) {
    float valueDec = getCurrent_raw();

    valueDec /= ina219_currentDivider_mA;

    return valueDec;
}

float ina_power_mW (void) {
    float valueDec = getPower_raw();
    valueDec *= ina219_powerMultiplier_mW;
    return valueDec;
}


