/*
 * PD50LE.cpp
 *
 *  Created on: Jun 20, 2024
 *      Author: NJunker
 */


#include "PD50LE_lib.h"

void Thorlabs_PD50LE::begin(uint8_t addr)
{
	_addr = addr;

	//Set LSB to 55mV for now, will add calibration later
	_lsb = 0.055;

	Thorlabs_I2C_setup();

}

bool Thorlabs_PD50LE::write_register(uint8_t reg, uint8_t data)
{
	uint8_t ret;
	uint8_t cmd[2];
	//build command word
	cmd[0] = reg;
	cmd[1] = data;

	Thorlabs_I2C_begin();
	ret = Thorlabs_I2C_write(cmd, sizeof(cmd));
	Thorlabs_I2C_end();

	return (ret > 0) ? false : true;

}

uint8_t Thorlabs_PD50LE::read_register(uint8_t reg)
{
	uint8_t cmd[1];
	uint8_t _read_buf[1];
	//build command word
	cmd[0] = reg;

	Thorlabs_I2C_begin();
	Thorlabs_I2C_write(cmd, sizeof(cmd));
	Thorlabs_I2C_end();
	Thorlabs_I2C_read(_read_buf, sizeof(_read_buf));
	
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

//-----------------------------------------------------------------------
//------------------- To be implemented by user -------------------------
//-----------------------(Platform Specific)-----------------------------
//-----------------------------------------------------------------------

uint8_t Thorlabs_PD50LE::Thorlabs_I2C_write(uint8_t *buf, size_t size) {
	//Implement this in a parent class or modify for your platform

	//Take in an array of single bytes (buf) and length
	//Return length of bytes successfully transmitted
}

void Thorlabs_PD50LE::Thorlabs_I2C_read(uint8_t *buf, size_t size) {
	//Implement this in a parent class or modify for your platform

	//Takes in a pointer to a buffer to place data in, and size of said buffer
}

void Thorlabs_PD50LE::Thorlabs_I2C_begin() {
	//Implement this in a parent class or modify for your platform

	//Used if your platform has an I2C transaction begin function (i.e. Arduino)
}

void Thorlabs_PD50LE::Thorlabs_I2C_end() {
	//Implement this in a parent class or modify for your platform
	
	//Used if your platform has an I2C transaction end function (i.e. Arduino)
}

void Thorlabs_PD50LE::Thorlabs_I2C_setup() {
	//Implement this in a parent class or modify for your platform

	//Platform specific startup code, i.e. setting pins, clock speed, etc
}


