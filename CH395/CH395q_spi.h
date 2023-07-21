#ifndef __CH395Q_SPI_H
#define __CH395Q_SPI_H

#include "main.h"

/* SPI�ӿڶ��� */
//#define CH395Q_SPI_INTERFACE                SPI3

/* IO���� */
#define CH395Q_SPI_CS(x)                    do{ x ?                                                                                         \
                                                    HAL_GPIO_WritePin(CH395Q_SPI_CS_GPIO_Port, CH395Q_SPI_CS_Pin, GPIO_PIN_SET) :  \
                                                    HAL_GPIO_WritePin(CH395Q_SPI_CS_GPIO_Port, CH395Q_SPI_CS_Pin, GPIO_PIN_RESET); \
                                                }while(0)

/* �������� */
void ch395q_spi_init(void);                 /* CH395Qģ��SPI�ӿڳ�ʼ�� */
static inline void ch395q_spi_start_cmd()   /* CH395Qģ��SPI��ʼ����� */
{
    CH395Q_SPI_CS(0);
}
static inline void ch395q_spi_end_cmd()     /* CH395Qģ��SPI��������� */
{
    CH395Q_SPI_CS(1);
}
void ch395q_spi_write_cmd(uint8_t cmd);     /* ��CH395Qģ��д���� */
void ch395q_spi_write_dat(uint8_t dat);     /* ��CH395Qģ��д���� */
uint8_t ch395q_spi_read_dat(void);          /* ��CH395Qģ������� */

#endif
