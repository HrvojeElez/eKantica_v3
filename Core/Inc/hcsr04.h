
#ifndef INC_HCSR04_H_
#define INC_HCSR04_H_

#define TRIG_PIN	GPIO_PIN_8
#define TRIG_PORT	GPIOE

#include "main.h"

extern uint32_t IC_Val1;
extern uint32_t IC_Val2;
extern uint32_t Difference;
extern uint8_t Is_First_Captured;
extern uint8_t Distance;

void delay(uint16_t time);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HCSR04_Read();

#endif /* INC_HCSR04_H_ */
