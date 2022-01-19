/*
 * sh.c
 *
 *  Created on: Jan 5, 2022
 *      Author: gugu
 */

/*
 * INCLUDES
 */
#include "sh.h"

/*
 * DEFINES
 */
#define DRY_SOIL	3455
#define WET_SOIL	1366

/*
 * INITIALISATION
 */
void SH_init(SH *dev, ADC_HandleTypeDef *ADC_handle) {
	dev->ADC_handle = ADC_handle;
}

/*
 * DATA ACQUISITION
 */
uint8_t SH_ReadData(SH *dev) {
	uint8_t errNum = 0;
	HAL_StatusTypeDef status;

	status = HAL_ADC_Start(dev->ADC_handle);
	errNum += (status != HAL_OK);

	status = HAL_ADC_PollForConversion(dev->ADC_handle, HAL_MAX_DELAY);
	errNum += (status != HAL_OK);

	dev->ADC_data = HAL_ADC_GetValue(dev->ADC_handle);
	HAL_ADC_Stop(dev->ADC_handle);
	errNum += (status != HAL_OK);

	if (dev->ADC_data >= DRY_SOIL) {
		dev->soilHumidity = 0.0;
	} else if (dev->ADC_data <= WET_SOIL) {
		dev->soilHumidity = 100.0;
	} else {
		dev->soilHumidity = ((100.0)/((double) WET_SOIL - (double) DRY_SOIL))*((double) (dev->ADC_data) - (double) DRY_SOIL);
	}

	return errNum;
}
