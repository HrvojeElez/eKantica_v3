/*
 * BME280 Combined Humidity and Pressure Sensor I2C Driver
 */

#ifndef INC_BME280_H_
#define INC_BME280_H_

#include "main.h"	/* Needed for I2C */

/*
 * DEFINES
 */
#define BME280_I2C_ADDR		(0x76 << 1)	/* SDO = 0 -> 0x76, SDO = 1 -> 0x77 (p. 32) */

#define BME280_DEVICE_ID	0x60

/*
 * REGISTERS (p. 26)
 */
#define BME280_REG_HUM_LSB		0xFE
#define BME280_REG_HUM_MSB		0xFD
#define BME280_REG_TEMP_XLSB	0xFC
#define BME280_REG_TEMP_LSB		0xFB
#define BME280_REG_TEMP_MSB		OxFA
#define BME280_REG_PRESS_XLSB	0xF9
#define BME280_REG_PRESS_LSB	0xF8
#define BME280_REG_PRESS_MSB	0xF7
#define BME280_REG_CONFIG		0xF5
#define BME280_REG_CTRL_MEAS	0xF4
#define BME280_REG_STATUS		0xF3
#define BME280_REG_CTRL_HUM		0xF2
#define BME280_REG_RESET		0xE0
#define	BME280_REG_ID			0xD0
#define BME280_REG_DIG_T1		0x88
#define BME280_REG_DIG_T2		0x8A
#define BME280_REG_DIG_T3		0x8C
#define BME280_REG_DIG_P1		0x8E
#define BME280_REG_DIG_P2		0x90
#define BME280_REG_DIG_P3		0x92
#define BME280_REG_DIG_P4		0x94
#define BME280_REG_DIG_P5		0x96
#define BME280_REG_DIG_P6		0x98
#define	BME280_REG_DIG_P7		0x9A
#define	BME280_REG_DIG_P8		0x9C
#define	BME280_REG_DIG_P9		0x9E
#define BME280_REG_DIG_H1		0xA0
#define BME280_REG_DIG_H2		0xE1
#define BME280_REG_DIG_H3		0xE3
#define BME280_REG_DIG_H4_MSB	0xE4
#define BME280_REG_DIG_H4_LSB	0xE5
#define BME280_REG_DIG_H5_MSB	0xE5
#define BME280_REG_DIG_H5_LSB	0xE6
#define BME280_REG_DIG_H6		0xE7

/*
 * SENSOR STRUCT
 */
typedef struct {
	/* I2C handle */
	I2C_HandleTypeDef *i2c_handle;

	/* Calibration data */
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;

	/* ADC Data */
	uint32_t ADC_t;
	uint32_t ADC_p;
	uint16_t ADC_h;

	/* Fine temperature */
	int32_t t_fine;

	/* Calibrated data */
	double temp;
	double press;
	double hum;
} BME280;

/*
 * CALIBRATION DATA EXTRACTION
 */
uint8_t BME280_ReadCalibrationData();

/*
 * INITIALISATION
 */
uint8_t BME280_Initialise(BME280 *dev, I2C_HandleTypeDef *i2c_handle);

/*
 * START MEASUREMENTS
 */
uint8_t BME280_PerformMeasurements(BME280 *dev);

/*
 * DATA ACQUISITION
 */
HAL_StatusTypeDef BME280_ReadData(BME280 *dev);

/*
 * DATA CALIBRATION
 */
double BME280_CalibrateTemp(BME280 *dev);
double BME280_CalibratePress(BME280 *dev);
double BME280_CalibrateHum(BME280 *dev);

/*
 * LOW LEVEL FUNCTIONS
 */
HAL_StatusTypeDef BME280_ReadRegister(BME280 *dev, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef BME280_ReadRegisters(BME280 *dev, uint8_t reg, uint8_t *data, uint8_t length);
HAL_StatusTypeDef BME280_WriteRegister(BME280 *dev, uint8_t reg, uint8_t *data);

#endif /* INC_BME280_H_ */
