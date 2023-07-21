#include "w5500_port_hal.h"

wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x11, 0x11, 0x11},
                            .ip = {192, 168, 1, 123},
                            .sn = {255,255,255,0},
                            .gw = {192, 168, 1, 1},
                            .dns = {8,8,8,8},
                            .dhcp = NETINFO_STATIC };

/**
 * @brief   enter critical section
 * @param   none
 * @return  none
 */
static void w5500_cris_enter(void)
{
    __set_PRIMASK(1);
}

/**
 * @brief   exit critical section
 * @param   none
 * @return  none
 */
static void w5500_cris_exit(void)
{
    __set_PRIMASK(0);
}

/**
 * @brief   select chip
 * @param   none
 * @return  none
 */
static void w5500_cs_select(void)
{
    HAL_GPIO_WritePin(W5500_CS_PORT, W5500_CS_PIN, GPIO_PIN_RESET);
}

/**
 * @brief   deselect chip
 * @param   none
 * @return  none
 */
static void w5500_cs_deselect(void)
{
    HAL_GPIO_WritePin(W5500_CS_PORT, W5500_CS_PIN, GPIO_PIN_SET);
}

/**
 * @brief   read byte in SPI interface
 * @param   none
 * @return  the value of the byte read
 */
static uint8_t w5500_spi_readbyte(void)
{
	uint8_t data[2] = {0xff, 0xff};
	HAL_SPI_TransmitReceive(&hspi3, data, data+1, 1, 100 );
	return data[1];
}

/**
 * @brief   write byte in SPI interface
 * @param   wb  the value to write
 * @return  none
 */
static void w5500_spi_writebyte(uint8_t wb)
{
		uint8_t data;
    HAL_SPI_TransmitReceive(&W5500_SPI_HANDLE, &wb, &data, 1, 100);
}

/**
 * @brief   burst read byte in SPI interface
 * @param   pBuf    pointer of data buf
 * @param   len     number of bytes to read
 * @return  none
 */
static void w5500_spi_readburst(uint8_t* pBuf, uint16_t len)
{
    if (!pBuf) {
        return;
    }
    
    HAL_SPI_Receive(&W5500_SPI_HANDLE, pBuf, len, 1000);
}

/**
 * @brief   burst write byte in SPI interface
 * @param   pBuf    pointer of data buf
 * @param   len     number of bytes to write
 * @return  none
 */
static void w5500_spi_writeburst(uint8_t* pBuf, uint16_t len)
{
    if (!pBuf) {
        return;
    }
    
    HAL_SPI_Transmit(&W5500_SPI_HANDLE, pBuf, len, 1000);
}

/**
 * @brief   hard reset
 * @param   none
 * @return  none
 */
static void w5500_hard_reset(void)
{
    HAL_GPIO_WritePin(W5500_RST_PORT, W5500_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(W5500_RST_PORT, W5500_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(50);
}

/**
  * @brief  Intialize the network information to be used in WIZCHIP
  * @retval None
  */
void network_init(void)
{
  uint8_t tmpstr[6];
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);

	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
	printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
		  gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	printf("======================\r\n");
}

/**
 * @brief   w5500 init
 * @param   none
 * @return  errcode
 * @retval  0   success
 * @retval  -1  chip init fail
 */
int w5500_init(void)
{
		uint8_t tmp;
		uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
			
    /* W5500 hard reset */
    w5500_hard_reset();
    
    /* Register spi driver function */
    reg_wizchip_cris_cbfunc(w5500_cris_enter, w5500_cris_exit);
    reg_wizchip_cs_cbfunc(w5500_cs_select, w5500_cs_deselect);
    reg_wizchip_spi_cbfunc(w5500_spi_readbyte, w5500_spi_writebyte);
    reg_wizchip_spiburst_cbfunc(w5500_spi_readburst, w5500_spi_writeburst);
		/* WIZCHIP SOCKET Buffer initialize */
		if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1){
			 printf("WIZCHIP Initialized fail.\r\n");
			 while(1);
		}

		/* PHY link status check */
		do{
			 if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1){
					printf("Unknown PHY Link stauts.\r\n");
			 }
		}while(tmp == PHY_LINK_OFF);
		
		/* Network initialization */
		network_init();
		
		return 0;
}

