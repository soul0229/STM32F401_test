#include "CH395q_spi.h"

/* 定义SPI句柄 */
//static SPI_HandleTypeDef g_ch395q_spi;
extern SPI_HandleTypeDef hspi3;
/**
 * @brief       CH395Q模块SPI写入并读出1字节数据
 * @param       tx_dat: 待写入的1个字节数据
 * @retval      读取出的1个字节数据
 */
static inline uint8_t ch395q_spi_write_read_byte(uint8_t tx_dat)
{
    uint8_t rx_dat;
    
    HAL_SPI_TransmitReceive(&hspi3, &tx_dat, &rx_dat, 1, HAL_MAX_DELAY);
    
    return rx_dat;
}

/**
 * @brief       向CH395Q模块写命令
 * @param       cmd: 待写入的命令码
 * @retval      无
 */
void ch395q_spi_write_cmd(uint8_t cmd)
{
    ch395q_spi_end_cmd();                   /* 防止CS原来为低，先将CS拉高 */
    ch395q_spi_start_cmd();                 /* 将CS拉低，开始写命令 */
    ch395q_spi_write_read_byte(cmd);        /* 通过SPI接口发送命令码 */
//    HAL_Delay(1);                                /* 必要延时，延时1.5us确保读写周期不小于1.5us */
}

/**
 * @brief       向CH395Q模块写数据
 * @param       dat: 待写入的数据
 * @retval      无
 */
void ch395q_spi_write_dat(uint8_t dat)
{
    ch395q_spi_write_read_byte(dat);        /* 通过SPI接口发送数据 */
}

/**
 * @brief       从CH395Q模块读数据
 * @param       无
 * @retval      读取到的数据
 */
uint8_t ch395q_spi_read_dat(void)
{
    uint8_t dat;
    
    dat = ch395q_spi_write_read_byte(0xFF); /* 通过SPI接口读取数据 */
    
    return dat;
}

