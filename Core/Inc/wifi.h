#ifndef INC_WIFI_H_
#define INC_WIFI_H_

#include "usart.h"

#include <stdio.h>
#include <string.h>

int8_t WIFI_Init(char *ssid, char *pwd);
int8_t WIFI_SendRequestWithParams(char *hostname, char *path, double temp, double moisture, double humidity, double waterLevel);
#endif /* INC_WIFI_H_ */
