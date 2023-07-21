#include "CH395q_spi.h"

/* ����SPI��� */
//static SPI_HandleTypeDef g_ch395q_spi;
extern SPI_HandleTypeDef hspi3;
/**
 * @brief       CH395Qģ��SPIд�벢����1�ֽ�����
 * @param       tx_dat: ��д���1���ֽ�����
 * @retval      ��ȡ����1���ֽ�����
 */
static inline uint8_t ch395q_spi_write_read_byte(uint8_t tx_dat)
{
    uint8_t rx_dat;
    
    HAL_SPI_TransmitReceive(&hspi3, &tx_dat, &rx_dat, 1, HAL_MAX_DELAY);
    
    return rx_dat;
}

/**
 * @brief       ��CH395Qģ��д����
 * @param       cmd: ��д���������
 * @retval      ��
 */
void ch395q_spi_write_cmd(uint8_t cmd)
{
    ch395q_spi_end_cmd();                   /* ��ֹCSԭ��Ϊ�ͣ��Ƚ�CS���� */
    ch395q_spi_start_cmd();                 /* ��CS���ͣ���ʼд���� */
    ch395q_spi_write_read_byte(cmd);        /* ͨ��SPI�ӿڷ��������� */
//    HAL_Delay(1);                                /* ��Ҫ��ʱ����ʱ1.5usȷ����д���ڲ�С��1.5us */
}

/**
 * @brief       ��CH395Qģ��д����
 * @param       dat: ��д�������
 * @retval      ��
 */
void ch395q_spi_write_dat(uint8_t dat)
{
    ch395q_spi_write_read_byte(dat);        /* ͨ��SPI�ӿڷ������� */
}

/**
 * @brief       ��CH395Qģ�������
 * @param       ��
 * @retval      ��ȡ��������
 */
uint8_t ch395q_spi_read_dat(void)
{
    uint8_t dat;
    
    dat = ch395q_spi_write_read_byte(0xFF); /* ͨ��SPI�ӿڶ�ȡ���� */
    
    return dat;
}

