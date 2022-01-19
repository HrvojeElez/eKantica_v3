/*
 * sh.h
 *
 *  Created on: Jan 5, 2022
 *      Author: gugu
 */

#ifndef INC_SH_H_
#define INC_SH_H_

/*
 * INCLUDES
 */
#include "main.h"

/*
 * SENSOR STRUCT
 */
typedef struct {
	/* ADC Handle*/
	ADC_HandleTypeDef *ADC_handle;

	/* Data from ADC */
	uint32_t ADC_data;

	/* Soil humidity */
	double soilHumidity;
} SH;

/*
 * INITIALISATION
 */
void SH_init(SH *dev, ADC_HandleTypeDef *ADC_handle);

/*
 * DATA ACQUISITION
 */
uint8_t SH_ReadData(SH *dev);

#endif /* INC_SH_H_ */
