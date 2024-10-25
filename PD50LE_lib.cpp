/*
 * PD50LE.cpp
 *
 *  Created on: Jun 20, 2024
 *      Author: NJunker
 */


#include "PD50LE_lib.h"
#include "math.h"

bool Thorlabs_PD50LE::begin(uint8_t addr, I2C_HandleTypeDef i2c_instance)
{
	_i2c_instance = i2c_instance;
	_addr = addr;

	//Set LSB to 55mV for now, will add calibration later
	_lsb = 0.055;

	HAL_StatusTypeDef ret;
	ret = HAL_I2C_IsDeviceReady(&_i2c_instance, _addr, 3, 1000);

	if (ret != HAL_OK) {
		return false;
	}
	else {
		return true;
	}
}

bool Thorlabs_PD50LE::write_register(uint8_t reg, uint8_t data)
{
	HAL_StatusTypeDef ret;
	uint8_t cmd[2];
	//build command word
	cmd[0] = reg;
	cmd[1] = data;

	ret = HAL_I2C_Master_Transmit(&_i2c_instance, (_addr << 1), cmd, sizeof(cmd), 1000);

	if (ret != HAL_OK) {
		return false;
	}
	else {
		return true;
	}
}

uint8_t Thorlabs_PD50LE::read_register(uint8_t reg)
{
	uint8_t cmd[1];
	uint8_t _read_buf[1];
	//build command word
	cmd[0] = reg;
	HAL_I2C_Master_Transmit(&_i2c_instance, (_addr << 1), cmd, sizeof(cmd), 100);
	HAL_I2C_Master_Receive(&_i2c_instance, (_addr << 1), _read_buf, sizeof(_read_buf), 100);
	return _read_buf[0];
}

bool Thorlabs_PD50LE::enable()
{
	return write_register(PD50LE_MODE_REGISTER, 0x01);
}

bool Thorlabs_PD50LE::disable()
{
	bool ret1;
	bool ret2;
	bool ret3;

	ret1 = write_register(PD50LE_DAC_MSB_REGISTER, 0x00);
	ret2 = write_register(PD50LE_DAC_LSB_REGISTER, 0x00);
	ret3 = write_register(PD50LE_MODE_REGISTER, 0x00);

	return (ret1 && ret2 && ret3);
}

bool Thorlabs_PD50LE::setDAC(uint16_t setting, polarity pol)
{
	bool ret1;
	bool ret2;

	uint8_t lsb = 0x00;
	uint8_t msb = 0x00;
	uint8_t polarityMask = 0x7F;

	msb = (setting >> 8) & 0xFF; //Shift to put bits 8-9 into spots 0-1
	msb &= polarityMask; //Isolate, reset polarity bit
	msb |= (pol << 7); //Set polarity bit
	lsb = (setting & 0xFF);

	ret1 = write_register(PD50LE_DAC_MSB_REGISTER, msb);
	ret2 = write_register(PD50LE_DAC_LSB_REGISTER, lsb);

	return ret1 && ret2;
}

bool Thorlabs_PD50LE::setVoltage(float voltage)
{
	uint16_t dacValue;
	polarity pol;

	dacValue = abs(voltage) / _lsb;

	if (signbit(voltage) == true){
		pol = NEGATIVE;
	} else {
		pol = POSITIVE;
	}

	//Limit available voltage from -10V to 50V, as listed in TLens silver datasheet
	if (-10 <= voltage && voltage <= 50){
		return setDAC(dacValue, pol);
	} else {
		return false;
	}


}

void Thorlabs_PD50LE::updateStatus()
{
	uint8_t data;

	data = read_register(PD50LE_STATUS_REGISTER);

	VOUT_LOCK = data & 1;
	DAC_ERRLTCH = (data >> 1) & 1;
}

bool Thorlabs_PD50LE::resetStatus()
{
	return write_register(PD50LE_STATUS_REGISTER, 0x00);
}


