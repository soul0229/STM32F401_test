#ifndef _W5500_PORT_HAL_
#define _W5500_PORT_HAL_

#include "wizchip_conf.h"
#include "stm32f4xx.h"
#include <string.h>
#include <stdio.h>

#define W5500_SPI_HANDLE    hspi3
#define W5500_CS_PORT       GPIOB
#define W5500_CS_PIN        GPIO_PIN_6
#define W5500_RST_PORT      GPIOB
#define W5500_RST_PIN       GPIO_PIN_8


extern SPI_HandleTypeDef W5500_SPI_HANDLE;

void w5500_network_info_show(void);
int w5500_init(void);

#endif
