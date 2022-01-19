/*
 * bme280.c
 *
 *  Created on: Jan 3, 2022
 *      Author: gugu
 */

#include "bme280.h"

/*
 * CALIBRATION DATA EXTRACTION
 */
uint8_t BME280_ReadCalibrationData(BME280 *dev) {

	/* Array in which will be stored calibration data*/
	uint8_t CalibrationData[32] = {0};

	uint8_t errNum = 0;
	HAL_StatusTypeDef status;

	/* Read calibration data from 0x88 to 0xA1 */
	status = BME280_ReadRegisters(dev, BME280_REG_DIG_T1, CalibrationData, 25);
	errNum += (status != HAL_OK);

	/* Read calibration data from 0xE1 to 0xE7 */
	status = BME280_ReadRegisters(dev, BME280_REG_DIG_H2, (uint8_t*) CalibrationData + 25, 7);
	errNum += (status != HAL_OK);

	dev->dig_T1 = ((CalibrationData[1] << 8) | CalibrationData[0]);
	dev->dig_T2 = ((CalibrationData[3] << 8) | CalibrationData[2]);
	dev->dig_T3 = ((CalibrationData[5] << 8) | CalibrationData[4]);

	dev->dig_P1 = ((CalibrationData[7] << 8) | CalibrationData[6]);
	dev->dig_P2 = ((CalibrationData[9] << 8) | CalibrationData[8]);
	dev->dig_P3 = ((CalibrationData[11] << 8) | CalibrationData[10]);
	dev->dig_P4 = ((CalibrationData[13] << 8) | CalibrationData[12]);
	dev->dig_P5 = ((CalibrationData[15] << 8) | CalibrationData[14]);
	dev->dig_P6 = ((CalibrationData[17] << 8) | CalibrationData[16]);
	dev->dig_P7 = ((CalibrationData[19] << 8) | CalibrationData[18]);
	dev->dig_P8 = ((CalibrationData[21] << 8) | CalibrationData[20]);
	dev->dig_P9 = ((CalibrationData[23] << 8) | CalibrationData[22]);

	dev->dig_H1 = CalibrationData[24];
	dev->dig_H2 = ((CalibrationData[26] << 8) | CalibrationData[25]);
	dev->dig_H3 = CalibrationData[27];
	dev->dig_H4 = ((CalibrationData[28] << 4) | (CalibrationData[29] & 0x0F));
	dev->dig_H5 = (((CalibrationData[29] & 0xF0) >> 4) | (CalibrationData[30] << 4));
	dev->dig_H6 = CalibrationData[31];

	return errNum;
}

/*
 * INITIALISATION
 */
uint8_t BME280_Initialise(BME280 *dev, I2C_HandleTypeDef *i2c_handle) {

	/* Set structure parameters */
	dev->i2c_handle	= i2c_handle;

	dev->ADC_t		= 0;
	dev->ADC_p		= 0;
	dev->ADC_h		= 0;

	dev->t_fine		= 0;

	dev->temp 		= 0.0f;
	dev->press		= 0.0f;
	dev->hum		= 0.0f;

	/* Store number of errors to be returned at the end of the function */
	uint8_t errNum = 0;
	HAL_StatusTypeDef status;

	/* Reset device */
	uint8_t regData;

	regData = 0xB6;
	status = BME280_WriteRegister(dev, BME280_REG_RESET, &regData);
	errNum += (status != HAL_OK);
	HAL_Delay(100);

	/*
	 * Check device ID (p. 27)
	 */

	status = BME280_ReadRegister(dev, BME280_REG_ID, &regData);
	errNum += (status != HAL_OK);

	if (regData != BME280_DEVICE_ID) {
		return 255;
	}

	/*
	 * Read calibration data
	 */
	errNum += BME280_ReadCalibrationData(dev);

	/*
	 * Set standby time (t_sb) to 0.5 milliseconds, IIR filter to off and disable SPI
	 */
	regData = 0x00;
	status = BME280_WriteRegister(dev, BME280_REG_CONFIG, &regData);
	errNum += (status != HAL_OK);

	/*
	 * Set humidity oversampling to x1
	 */
	regData = 0x01;
	status = BME280_WriteRegister(dev, BME280_REG_CTRL_HUM, &regData);
	errNum += (status != HAL_OK);

	/*
	 * Set temperature oversampling to x1, pressure oversampling to x1 and mode to Forced
	 */
	regData = 0x26;
	status = BME280_WriteRegister(dev, BME280_REG_CTRL_MEAS, &regData);
	errNum += (status != HAL_OK);

	return errNum;
}

/*
 * START MEASUREMENTS
 */
uint8_t BME280_PerformMeasurements(BME280 *dev) {

	uint8_t regData = 0;
	uint8_t errNum = 0;
	HAL_StatusTypeDef status;

	/* Read CTRL_MEAS register */
	status = BME280_ReadRegister(dev, BME280_REG_CTRL_MEAS, &regData);
	errNum += (status != HAL_OK);

	/* Set device to Forced mode*/
	regData = regData | 0x01;
	BME280_WriteRegister(dev, BME280_REG_CTRL_MEAS, &regData);
	errNum += (status != HAL_OK);

	HAL_Delay(100);

	return errNum;
}

/*
 * DATA ACQUISITION
 */
HAL_StatusTypeDef BME280_ReadData(BME280 *dev) {

	uint8_t ADC_data[8] = {0};
	HAL_StatusTypeDef status;

	status = BME280_ReadRegisters(dev, BME280_REG_PRESS_MSB, ADC_data, 8);

	dev->ADC_p = ((ADC_data[0] << 12) | (ADC_data[1] << 4) | (ADC_data[2] >> 4));
	dev->ADC_t = ((ADC_data[3] << 12) | (ADC_data[4] << 4) | (ADC_data[5] >> 4));
	dev->ADC_h = ((ADC_data[6] << 8) | ADC_data[7]);

	dev->temp = BME280_CalibrateTemp(dev);
	dev->press = BME280_CalibratePress(dev);
	dev->hum = BME280_CalibrateHum(dev);

	return status;
}

/*
 * DATA CALIBRATION
 */
double BME280_CalibrateTemp(BME280 *dev) {
	double var1, var2, T;

	var1 = (((double) (dev->ADC_t)) / 16384.0 - ((double) (dev->dig_T1)) / 1024.0) * ((double) (dev->dig_T2));
	var2 = ((((double) (dev->ADC_t)) / 131072.0 - ((double) (dev->dig_T1)) / 8192.0) *
			(((double) (dev->ADC_t)) / 131072.0 - ((double) (dev->dig_T1)) / 8192.0)) * ((double) (dev->dig_T3));

	dev->t_fine = (int32_t)(var1 + var2);
	T = (var1 + var2) / 5120.0;

	return T;
}

double BME280_CalibratePress(BME280 *dev) {
	double var1, var2, p;

	var1 = ((double) (dev->t_fine) / 2.0) - 64000.0;
	var2 = var1 * var1 * ((double) (dev->dig_P6)) / 32768.0;
	var2 = var2 + var1 * ((double) (dev->dig_P5)) * 2.0;
	var2 = (var2 / 4.0)+(((double) (dev->dig_P4)) * 65536.0);
	var1 = (((double) (dev->dig_P3)) * var1 * var1 / 524288.0 + ((double) (dev->dig_P2)) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0)*((double) (dev->dig_P1));

	if (var1 == 0.0) {
		return 0;
	}

	p = 1048576.0 - (double) (dev->ADC_p);
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double) (dev->dig_P9)) * p * p / 2147483648.0;
	var2 = p * ((double) (dev->dig_P8)) / 32768.0;
	p = p + (var1 + var2 + ((double) (dev->dig_P7))) / 16.0;

	return p;
}

double BME280_CalibrateHum(BME280 *dev) {
	double var_H;

	var_H = (((double) (dev->t_fine)) - 76800.0);
	var_H = ((dev->ADC_h) - (((double) (dev->dig_H4)) * 64.0 + ((double) (dev->dig_H5)) / 16384.0 *
			var_H)) * (((double) (dev->dig_H2)) / 65536.0 * (1.0 + ((double) (dev->dig_H6)) /
					67108864.0 * var_H * (1.0 + ((double) (dev->dig_H3)) / 67108864.0 * var_H)));
	var_H = var_H * (1.0 - ((double) (dev->dig_H1)) * var_H / 524288.0);

	if (var_H > 100.0) {
		var_H = 100.0;
	} else if (var_H < 0.0) {
		var_H = 0.0;
	}

	return var_H;
}

/*
 * LOW LEVEL FUNCTIONS
 */
HAL_StatusTypeDef BME280_ReadRegister(BME280 *dev, uint8_t reg, uint8_t *data) {
	return HAL_I2C_Mem_Read(dev->i2c_handle, BME280_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef BME280_ReadRegisters(BME280 *dev, uint8_t reg, uint8_t *data, uint8_t length) {
	return HAL_I2C_Mem_Read(dev->i2c_handle, BME280_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

HAL_StatusTypeDef BME280_WriteRegister(BME280 *dev, uint8_t reg, uint8_t *data) {
	return HAL_I2C_Mem_Write(dev->i2c_handle, BME280_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}
