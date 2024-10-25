/*
 * PD50LE.h
 *
 *  Created on: Jun 20, 2024
 *      Author: NJunker
 *      WIP Library for the poLight PD50LE TLens driver ASIC
 *      (this one is VERY work in progress, will be adding features down the road)
 */

#ifndef INC_PD50LE_LIB_H_
#define INC_PD50LE_LIB_H_

#include <cstdint> //for uint8_t, etc
#include <cstddef> //for size_t
#include <math.h> //for abs, signbit, etc

//extern I2C_HandleTypeDef hi2c1;

#define PD50LE_DEFAULT_ADDR 0x0C

#define PD50LE_MODE_REGISTER 0x00
#define PD50LE_SPEED_REGISTER 0x01
#define PD50LE_DAC_MSB_REGISTER 0x02
#define PD50LE_DAC_LSB_REGISTER 0x03
#define PD50LE_STATUS_REGISTER 0x04
#define PD50LE_CHIP_ID_REGISTER 0x07

//TODO: Figure out why PD50LE occasionally gets stuck and can't enable. Reproducible by spamming reset pin.
//TODO: Add calibration functions, clean up library

class Thorlabs_PD50LE {

public:

	typedef enum {
		NEGATIVE,
		POSITIVE
	} polarity;

	bool VOUT_LOCK;
	bool DAC_ERRLTCH;

	//Initialize object with I2C instance and address
	void begin(uint8_t addr = PD50LE_DEFAULT_ADDR);

	//Enable the driver
	bool enable();

	//Disable the driver to save power
	bool disable();

	//Set 10-bit DAC to a value within the range 0-1023.
	bool setDAC(uint16_t setting, polarity pol);

	//Set voltage level as float. Will be calculated into 10-bit DAC value.
	bool setVoltage(float voltage);

	//Reset status register. Use to reset the DAC write error latch.
	bool resetStatus();

	//Read status register. Updates VOUT_LOCK and DAC_ERRLTCH bits.
	void updateStatus();

protected:

	uint8_t _addr;
	float _lsb;

	uint8_t Thorlabs_I2C_write(uint8_t *buf, size_t size) __attribute__((weak));

	void Thorlabs_I2C_read(uint8_t *buf, size_t size) __attribute__((weak));

	void Thorlabs_I2C_begin() __attribute__((weak));

	void Thorlabs_I2C_end() __attribute__((weak));

	void Thorlabs_I2C_setup() __attribute__((weak));


private:

	bool write_register(uint8_t reg, uint8_t data);
	uint8_t read_register(uint8_t reg);
};


#endif /* THORLABS_PD50LE_INC_PD50LE_LIB_H_ */
