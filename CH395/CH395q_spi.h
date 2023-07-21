#ifndef __CH395Q_SPI_H
#define __CH395Q_SPI_H

#include "main.h"

/* SPI接口定义 */
//#define CH395Q_SPI_INTERFACE                SPI3

/* IO操作 */
#define CH395Q_SPI_CS(x)                    do{ x ?                                                                                         \
                                                    HAL_GPIO_WritePin(CH395Q_SPI_CS_GPIO_Port, CH395Q_SPI_CS_Pin, GPIO_PIN_SET) :  \
                                                    HAL_GPIO_WritePin(CH395Q_SPI_CS_GPIO_Port, CH395Q_SPI_CS_Pin, GPIO_PIN_RESET); \
                                                }while(0)

/* 操作函数 */
void ch395q_spi_init(void);                 /* CH395Q模块SPI接口初始化 */
static inline void ch395q_spi_start_cmd()   /* CH395Q模块SPI开始命令传输 */
{
    CH395Q_SPI_CS(0);
}
static inline void ch395q_spi_end_cmd()     /* CH395Q模块SPI结束命令传输 */
{
    CH395Q_SPI_CS(1);
}
void ch395q_spi_write_cmd(uint8_t cmd);     /* 向CH395Q模块写命令 */
void ch395q_spi_write_dat(uint8_t dat);     /* 向CH395Q模块写数据 */
uint8_t ch395q_spi_read_dat(void);          /* 从CH395Q模块读数据 */

#endif
