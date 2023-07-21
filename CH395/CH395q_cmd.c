#include "CH395q_cmd.h"
#include "CH395q_spi.h"
#include "CH395q.h"

/* �����붨�� */
#define CH395Q_CMD_GET_IC_VER               0x01    /* ��ȡоƬ���̼��汾�� */
#define CH395Q_CMD_SET_BAUDRATE             0x02    /* ���ô���ͨѶ������ */
#define CH395Q_CMD_ENTER_SLEEP              0x03    /* ����͹���˯�߹���״̬ */
#define CH395Q_CMD_RESET_ALL                0x05    /* ִ��Ӳ����λ */
#define CH395Q_CMD_CHECK_EXIST              0x06    /* ����ͨѶ�ӿں͹�����ʽ */
#define CH395Q_CMD_GET_GLOB_INT_STATUS_ALL  0x19    /* ��ȡȫ���ж�״̬ */
#define CH395Q_CMD_SET_PHY                  0x20    /* ����PHY���ӷ�ʽ */
#define CH395Q_CMD_SET_MAC_ADDR             0x21    /* ����MAC��ַ */
#define CH395Q_CMD_SET_IP_ADDR              0x22    /* ����IP��ַ */
#define CH395Q_CMD_SET_GWIP_ADDR            0x23    /* ��������IP��ַ */
#define CH395Q_CMD_SET_MASK_ADDR            0x24    /* ������������ */
#define CH395Q_CMD_SET_MAC_FILT             0x25    /* ����MAC����ģʽ */
#define CH395Q_CMD_GET_PHY_STATUS           0x26    /* ��ȡPHY��״̬ */
#define CH395Q_CMD_INIT_CH395               0x27    /* ��ʼ��CH395QоƬ */
#define CH395Q_CMD_GET_UNREACH_IPPORT       0x28    /* ��ȡ���ɴ�IP���˿ں�Э�� */
#define CH395Q_CMD_GET_GLOB_INT_STATUS      0x29    /* ��ȡȫ���ж�״̬ */
#define CH395Q_CMD_SET_RETRAN_COUNT         0x2A    /* �������Դ��������20�� */
#define CH395Q_CMD_SET_RETRAN_PERIOD        0x2B    /* �����������ڣ����100ms */
#define CH395Q_CMD_GET_CMD_STATUS           0x2C    /* ��ȡ����ִ��״̬ */
#define CH395Q_CMD_GET_REMOT_IPP_SN         0x2D    /* ��ȡԶ�˵�IP�Ͷ˿� */
#define CH395Q_CMD_CLEAR_RECV_BUF_SN        0x2E    /* ���Socket�Ľ��ջ����� */
#define CH395Q_CMD_GET_SOCKET_STATUS_SN     0x2F    /* ��ȡSocket״̬ */
#define CH395Q_CMD_GET_INT_STATUS_SN        0x30    /* ��ȡSocket���ж�״̬ */
#define CH395Q_CMD_SET_IP_ADDR_SN           0x31    /* ����Socket��Ŀ��IP��ַ */
#define CH395Q_CMD_SET_DES_PORT_SN          0x32    /* ����Socket��Ŀ�Ķ˿� */
#define CH395Q_CMD_SET_SOUR_PORT_SN         0x33    /* ����Socket��Դ�˿� */
#define CH395Q_CMD_SET_PROTO_TYPE_SN        0x34    /* ����Socket�Ĺ���ģʽ */
#define CH395Q_CMD_OPEN_SOCKET_SN           0x35    /* ��Socket */
#define CH395Q_CMD_TCP_LISTEN_SN            0x36    /* ����Socket���� */
#define CH395Q_CMD_TCP_CONNECT_SN           0x37    /* ����Socket���� */
#define CH395Q_CMD_TCP_DISNCONNECT_SN       0x38    /* �Ͽ�Socket��TCP���� */
#define CH395Q_CMD_WRITE_SEND_BUF_SN        0x39    /* ��Socket���ͻ�����д���� */
#define CH395Q_CMD_GET_RECV_LEN_SN          0x3B    /* ��ȡSocket�������ݳ��� */
#define CH395Q_CMD_READ_RECV_BUF_SN         0x3C    /* ��Socket���ջ������������� */
#define CH395Q_CMD_CLOSE_SOCKET_SN          0x3D    /* �ر�Socket */
#define CH395Q_CMD_SET_IPRAW_PRO_SN         0x3E    /* ����Socket��IP����Э���ֶ� */
#define CH395Q_CMD_PING_ENABLE              0x3F    /* PINGʹ�� */
#define CH395Q_CMD_GET_MAC_ADDR             0x40    /* ��ȡMAC��ַ */
#define CH395Q_CMD_DHCP_ENABLE              0x41    /* ������ֹͣ��DHCP */
#define CH395Q_CMD_GET_DHCP_STATUS          0x42    /* ��ȡDHCP״̬ */
#define CH395Q_CMD_GET_IP_INF               0x43    /* ��ȡIP��MASK��DNS����Ϣ */
#define CH395Q_CMD_PPPOE_SET_USER_NAME      0x44    /* ����PPPOE�û��� */
#define CH395Q_CMD_PPPOE_SET_PASSWORD       0x45    /* ����PPPOE���� */
#define CH395Q_CMD_PPPOE_ENABLE             0x46    /* ������ֹͣ��PPPOE */
#define CH395Q_CMD_GET_PPPOE_STATUS         0x47    /* ��ȡPPPOE״̬ */
#define CH395Q_CMD_SET_TCP_MSS              0x50    /* ����TCP MSS */
#define CH395Q_CMD_SET_TTL                  0x51    /* ����TTLֵ�����128 */
#define CH395Q_CMD_SET_RECV_BUF             0x52    /* ����Socket���ջ����� */
#define CH395Q_CMD_SET_SEND_BUF             0x53    /* ����Socket���ͻ����� */
#define CH395Q_CMD_SET_MAC_RECV_BUF         0x54    /* ����MAC���ջ����� */
#define CH395Q_CMD_SET_FUN_PARA             0x55    /* ���ù��ܲ��� */
#define CH395Q_CMD_SET_KEEP_LIVE_IDLE       0x56    /* ����Keeplive����ʱ�� */
#define CH395Q_CMD_SET_KEEP_LIVE_INTVL      0x57    /* ����Keeplive��ʱʱ�� */
#define CH395Q_CMD_SET_KEEP_LIVE_CNT        0x58    /* ����Keeplive��ʱ���Դ��� */
#define CH395Q_CMD_SET_KEEP_LIVE_SN         0X59    /* ����Socket Keeplive */
#define CH395Q_CMD_EEPROM_ERASE             0xE9    /* ����EEPROM */
#define CH395Q_CMD_EEPROM_WRITE             0xEA    /* дEEPROM */
#define CH395Q_CMD_EEPROM_READ              0xEB    /* ��EEPROM */
#define CH395Q_CMD_READ_GPIO_REG            0xEC    /* ��GPIO�Ĵ��� */
#define CH395Q_CMD_WRITE_GPIO_REG           0xED    /* дGPIO�Ĵ��� */

/**
 * @brief       ��ȡоƬ���̼��汾
 * @param       ��
 * @retval      оƬ���̼��汾��
 *              bit[7:6]: 0b01
 *              bit[5:0]: �汾��
 */
uint8_t ch395q_cmd_get_ic_version(void)
{
    uint8_t version;
    
    ch395q_spi_write_cmd(CH395Q_CMD_GET_IC_VER);
    version = ch395q_spi_read_dat();
    ch395q_spi_end_cmd();
    
    return version;
}

/**
 * @brief       ���ô���ͨѶ������
 * @note        ���ڴ���ģʽ����Ч
 * @param       baudrate: ���ڲ�����ϵ������������ڲ�����ϵ����
 * @retval      ��
 */
void ch395q_cmd_set_baudrate(uint32_t baudrate)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_BAUDRATE);
    ch395q_spi_write_dat((uint8_t)baudrate & 0xFF);
    ch395q_spi_write_dat((uint8_t)(baudrate >> 8) & 0xFF);
    ch395q_spi_write_dat((uint8_t)(baudrate >> 16) & 0xFF);
    ch395q_spi_read_dat();
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����͹���˯�߹���״̬
 * @param       ��
 * @retval      ��
 */
void ch395q_cmd_enter_sleep(void)
{
    ch395q_spi_write_cmd(CH395Q_CMD_ENTER_SLEEP);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ִ��Ӳ����λ
 * @param       ��
 * @retval      ��
 */
void ch395q_cmd_reset_all(void)
{
    ch395q_spi_write_cmd(CH395Q_CMD_RESET_ALL);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ��ȡȫ���ж�״̬
 * @param       ��
 * @retval      ��ǰȫ���ж�״̬�������ȫ���ж�״̬��
 */
uint16_t ch395q_cmd_get_glob_int_status_all(void)
{
    uint16_t status;
    
    ch395q_spi_write_cmd(CH395Q_CMD_GET_GLOB_INT_STATUS_ALL);
//		HAL_Delay(2);
    status = (uint16_t)ch395q_spi_read_dat() & 0x00FF;
    status |= ((uint16_t)ch395q_spi_read_dat() << 8) & 0xFF00;
    ch395q_spi_end_cmd();
    
    return status;
}

/**
 * @brief       ����ͨѶ�ӿں͹���״̬
 * @note        ��ͨѶ�ӿ�ͨѶ���󣬽����ز�������ֵ�İ�λȡ�����ֵ
 * @param       tx_dat: ����ֵ����������
 * @retval      ���Խ��ֵ
 */
uint8_t ch395q_cmd_check_exist(uint8_t tx_dat)
{
    uint8_t rx_dat;
    
    ch395q_spi_write_cmd(CH395Q_CMD_CHECK_EXIST);
    ch395q_spi_write_dat(tx_dat);
    rx_dat = ch395q_spi_read_dat();
    ch395q_spi_end_cmd();
    
    return rx_dat;
}

/**
 * @brief       ����PHY���ӷ�ʽ
 * @param       phy_type: �ο���PHY���ӷ�ʽ��
 * @retval      ��
 */
void ch395q_cmd_set_phy(uint8_t phy_type)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_PHY);
    ch395q_spi_write_dat(phy_type);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����MAC��ַ
 * @param       mac_addr: ָ����MAC��ַ
 * @retval      ��
 */
void ch395q_cmd_set_mac_addr(uint8_t *mac_addr)
{
    uint8_t mac_index;
    
    ch395q_spi_write_cmd(CH395Q_CMD_SET_MAC_ADDR);
    for (mac_index=0; mac_index<6; mac_index++)
    {
        ch395q_spi_write_dat(mac_addr[mac_index]);
    }
    ch395q_spi_end_cmd();
    HAL_Delay(100);
}

/**
 * @brief       ����IP��ַ
 * @param       ip_addr: ָ����IP��ַ
 * @retval      ��
 */
void ch395q_cmd_set_ip_addr(uint8_t *ip_addr)
{
    uint8_t ip_index;
    
    ch395q_spi_write_cmd(CH395Q_CMD_SET_IP_ADDR);
    for (ip_index=0; ip_index<4; ip_index++)
    {
        ch395q_spi_write_dat(ip_addr[ip_index]);
    }
    ch395q_spi_end_cmd();
}

/**
 * @brief       ��������IP��ַ
 * @param       gwip_addr: ָ��������IP��ַ
 * @retval      ��
 */
void ch395q_cmd_set_gwip_addr(uint8_t *gwip_addr)
{
    uint8_t gwip_index;
    
    ch395q_spi_write_cmd(CH395Q_CMD_SET_GWIP_ADDR);
    for (gwip_index=0; gwip_index<4; gwip_index++)
    {
        ch395q_spi_write_dat(gwip_addr[gwip_index]);
    }
    ch395q_spi_end_cmd();
}

/**
 * @brief       ������������
 * @note        ��������Ĭ��Ϊ255.255.255.0
 * @param       mask_addr: ָ������������
 * @retval      ��
 */
void ch395q_cmd_set_mask_addr(uint8_t *mask_addr)
{
    uint8_t mask_index;
    
    ch395q_spi_write_cmd(CH395Q_CMD_SET_MASK_ADDR);
    for (mask_index=0; mask_index<4; mask_index++)
    {
        ch395q_spi_write_dat(mask_addr[mask_index]);
    }
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����MAC����ģʽ
 * @param       filt_mode: ����ģʽ���ο���MAC����ģʽ��
 *              table0   : HASH0
 *              table1   : HASH1
 * @retval      ��
 */
void ch395q_cmd_set_mac_filt(uint8_t filt_mode, uint32_t table0, uint32_t table1)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_MAC_FILT);
    ch395q_spi_write_dat(filt_mode);
    ch395q_spi_write_dat((uint8_t)table0 & 0xFF);
    ch395q_spi_write_dat((uint8_t)(table0 >> 8) & 0xFF);
    ch395q_spi_write_dat((uint8_t)(table0 >> 16) & 0xFF);
    ch395q_spi_write_dat((uint8_t)(table0 >> 24) & 0xFF);
    ch395q_spi_write_dat((uint8_t)table1 & 0xFF);
    ch395q_spi_write_dat((uint8_t)(table1 >> 8) & 0xFF);
    ch395q_spi_write_dat((uint8_t)(table1 >> 16) & 0xFF);
    ch395q_spi_write_dat((uint8_t)(table1 >> 24) & 0xFF);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ��ȡPHY��״̬
 * @param       ��
 * @retval      ��ǰPHY��״̬���ο���PHY���ӷ�ʽ��
 */
uint8_t ch395q_cmd_get_phy_status(void)
{
    uint8_t status;
    
    ch395q_spi_write_cmd(CH395Q_CMD_GET_PHY_STATUS);
    status = ch395q_spi_read_dat();
    ch395q_spi_end_cmd();
    
    return status;
}

/**
 * @brief       ��ʼ��CH395оƬ
 * @note        ��Ҫ����500ms��ʱ������ɳ�ʼ��
 * @param       ��
 * @retval      CH395Q_EOK  : CH395QоƬ��ʼ���ɹ�
 *              CH395Q_ERROR: CH395QоƬ��ʼ��ʧ��
 */
uint8_t ch395q_cmd_init_ch395(void)
{
    uint8_t status;
    uint8_t timeout = 0;
    
    ch395q_spi_write_cmd(CH395Q_CMD_INIT_CH395);
    ch395q_spi_end_cmd();
    while (1)
    {
        HAL_Delay(10);
        status = ch395q_cmd_get_cmd_status();
        if ((status != CH395Q_CMD_ERR_BUSY) || (timeout++ > 200))
        {
            break;
        }
    }
    
    if (status != CH395Q_CMD_ERR_SUCCESS)
    {
        return CH395Q_ERROR;
    }
    
    return CH395Q_EOK;
}

/**
 * @brief       ��ȡ���ɴ�IP���˿ں�Э��
 * @param       unreach_code : ���ɴ���룬�ο������ɴ���롱
 *              proto_type   : Э������
 *              port         : �˿ں�
 *              ip_addr      : IP��ַ
 * @retval      ��
 */
void ch395q_cmd_get_unreach_ipport(uint8_t *unreach_code, uint8_t *proto_type, uint16_t *port, uint8_t *ip_addr)
{
    ch395q_spi_write_cmd(CH395Q_CMD_GET_UNREACH_IPPORT);
    *unreach_code = ch395q_spi_read_dat();
    *proto_type = ch395q_spi_read_dat();
    *port = (uint16_t)ch395q_spi_read_dat() & 0x00FF;
    *port |= ((uint16_t)ch395q_spi_read_dat() << 8) & 0xFF00;
    ip_addr[0] = ch395q_spi_read_dat();
    ip_addr[1] = ch395q_spi_read_dat();
    ip_addr[2] = ch395q_spi_read_dat();
    ip_addr[3] = ch395q_spi_read_dat();
    ch395q_spi_end_cmd();
}

/**
 * @brief       ��ȡȫ���ж�״̬
 * @note        �յ���������Զ�ȡ���ж�
 * @param       ��
 * @retval      ��ǰ��ȫ���ж�״̬�������ȫ���ж�״̬��
 */
uint8_t ch395q_cmd_get_glob_int_status(void)
{
    uint8_t status;
    
    ch395q_spi_write_cmd(CH395Q_CMD_GET_GLOB_INT_STATUS);
    status = ch395q_spi_read_dat();
    ch395q_spi_end_cmd();
    
    return status;
}

/**
 * @brief       �������Դ���
 * @note        ������Դ���Ϊ20��
 * @param       retran_count: ���Դ���
 * @retval      ��
 */
void ch395q_cmd_set_retran_count(uint8_t retran_count)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_RETRAN_COUNT);
    ch395q_spi_write_dat(retran_count);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ������������
 * @note        ������Դ���Ϊ1000����
 * @param       retran_period: ���Դ���
 * @retval      ��
 */
void ch395q_cmd_set_retran_period(uint16_t retran_period)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_RETRAN_PERIOD);
    ch395q_spi_write_dat(retran_period);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ��ȡ����ִ��״̬
 * @note        ĳЩ������Ҫ�ȴ�����ִ�н��
 * @param       ��
 * @retval      ��һ�������ִ��״̬���ο�������ִ��״̬��
 */
uint8_t ch395q_cmd_get_cmd_status(void)
{
    uint8_t status;
    
    ch395q_spi_write_cmd(CH395Q_CMD_GET_CMD_STATUS);
    status = ch395q_spi_read_dat();
    ch395q_spi_end_cmd();
    
    return status;
}

/**
 * @brief       ��ȡԶ��(Ŀ��)�� IP �Ͷ˿�
 * @param       socket : ָ����Socket
 *              ip_addr: IP��ַ
 *              port   : �˿�
 * @retval      ��
 */
void ch395q_cmd_get_remot_ipp_sn(uint8_t socket, uint8_t *ip_addr, uint16_t *port)
{
    ch395q_spi_write_cmd(CH395Q_CMD_GET_REMOT_IPP_SN);
    ch395q_spi_write_dat(socket);
    ip_addr[0] = ch395q_spi_read_dat();
    ip_addr[1] = ch395q_spi_read_dat();
    ip_addr[2] = ch395q_spi_read_dat();
    ip_addr[3] = ch395q_spi_read_dat();
    *port = (uint16_t)ch395q_spi_read_dat() & 0x00FF;
    *port |= ((uint16_t)ch395q_spi_read_dat() << 8) & 0xFF00;
    ch395q_spi_end_cmd();
}

/**
 * @brief       ���Socket�Ľ��ջ�����
 * @param       socket: ָ����Socket
 * @retval      ��
 */
void ch395q_cmd_clear_recv_buf_sn(uint8_t socket)
{
    ch395q_spi_write_cmd(CH395Q_CMD_CLEAR_RECV_BUF_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ��ȡSocket״̬
 * @param       socket       : ָ����Socket
 *              socket_status: Socket��״̬�룬�����Socket״̬�롱
 *              tcp_status   : TCP��״̬�룬�����TCP״̬�롱
 * @retval      ��
 */
void ch395q_cmd_get_socket_status_sn(uint8_t socket, uint8_t *socket_status, uint8_t *tcp_status)
{
    ch395q_spi_write_cmd(CH395Q_CMD_GET_SOCKET_STATUS_SN);
    ch395q_spi_write_dat(socket);
    *socket_status = ch395q_spi_read_dat();
    *tcp_status = ch395q_spi_read_dat();
    ch395q_spi_end_cmd();
}

/**
 * @brief       ��ȡSocket���ж�״̬
 * @param       socket: ָ����Socket
 * @retval      Socket���ж�״̬�������Socket�ж�״̬��
 */
uint8_t ch395q_cmd_get_int_status_sn(uint8_t socket)
{
    uint8_t status;
    
    ch395q_spi_write_cmd(CH395Q_CMD_GET_INT_STATUS_SN);
    ch395q_spi_write_dat(socket);
//    HAL_Delay(2);
    status = ch395q_spi_read_dat();
    ch395q_spi_end_cmd();
    
    return status;
}

/**
 * @brief       ����Socket��Ŀ��IP��ַ
 * @param       socket: ָ����Socket
 *              ip_addr: Socket��Ŀ��IP��ַ
 * @retval      ��
 */
void ch395q_cmd_set_ip_addr_sn(uint8_t socket, uint8_t *ip_addr)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_IP_ADDR_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_write_dat(ip_addr[0]);
    ch395q_spi_write_dat(ip_addr[1]);
    ch395q_spi_write_dat(ip_addr[2]);
    ch395q_spi_write_dat(ip_addr[3]);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����Socket��Ŀ�Ķ˿�
 * @param       socket: ָ����Socket
 *              port: Socket��Ŀ�Ķ˿�
 * @retval      ��
 */
void ch395q_cmd_set_des_port_sn(uint8_t socket, uint16_t port)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_DES_PORT_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_write_dat((uint8_t)port & 0xFF);
    ch395q_spi_write_dat((uint8_t)(port >> 8) & 0xFF);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����Socket��Դ�˿�
 * @param       socket: ָ����Socket
 *              port: Socket��Ŀ�Ķ˿�
 * @retval      ��
 */
void ch395q_cmd_set_sour_port_sn(uint8_t socket, uint16_t port)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_SOUR_PORT_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_write_dat((uint8_t)port & 0xFF);
    ch395q_spi_write_dat((uint8_t)(port >> 8) & 0xFF);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����Socket�Ĺ���ģʽ
 * @param       socket : ָ����Socket
 *              socket_proto: Socket��Э�����ͣ������SocketЭ�����͡�
 * @retval      ��
 */
void ch395q_cmd_set_proto_type_sn(uint8_t socket, uint8_t socket_proto)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_PROTO_TYPE_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_write_dat(socket_proto);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ��Socket
 * @param       socket: ָ����Socket
 * @retval      CH395Q_EOK  : Socket�򿪳ɹ�
 *              CH395Q_ERROR: Socket��ʧ��
 */
uint8_t ch395q_cmd_open_socket_sn(uint8_t socket)
{
    uint8_t status;
    uint8_t timeout = 0;
    
    ch395q_spi_write_cmd(CH395Q_CMD_OPEN_SOCKET_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_end_cmd();
    while (1)
    {
        HAL_Delay(5);
        status = ch395q_cmd_get_cmd_status();
        if ((status != CH395Q_CMD_ERR_BUSY) || (timeout++ > 200))
        {
            break;
        }
    }
    
    if (status != CH395Q_CMD_ERR_SUCCESS)
    {
        return CH395Q_ERROR;
    }
    
    return CH395Q_EOK;
}

/**
 * @brief       ����Socket����
 * @param       socket: ָ����Socket
 * @retval      CH395Q_EOK  : Socket���������ɹ�
 *              CH395Q_ERROR: Socket��������ʧ��
 */
uint8_t ch395q_cmd_tcp_listen_sn(uint8_t socket)
{
    uint8_t status;
    uint8_t timeout = 0;
    
    ch395q_spi_write_cmd(CH395Q_CMD_TCP_LISTEN_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_end_cmd();
    while (1)
    {
        HAL_Delay(5);
        status = ch395q_cmd_get_cmd_status();
        if ((status != CH395Q_CMD_ERR_BUSY) || (timeout++ > 200))
        {
            break;
        }
    }
    
    if (status != CH395Q_CMD_ERR_SUCCESS)
    {
        return CH395Q_ERROR;
    }
    
    return CH395Q_EOK;
}

/**
 * @brief       ����Socket����
 * @param       socket: ָ����Socket
 * @retval      CH395Q_EOK  : Socket���������ɹ�
 *              CH395Q_ERROR: Socket��������ʧ��
 */
uint8_t ch395q_cmd_tcp_connect_sn(uint8_t socket)
{
    uint8_t status;
    uint8_t timeout = 0;
    
    ch395q_spi_write_cmd(CH395Q_CMD_TCP_CONNECT_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_end_cmd();
    while (1)
    {
        HAL_Delay(5);
        status = ch395q_cmd_get_cmd_status();
        if ((status != CH395Q_CMD_ERR_BUSY) || (timeout++ > 200))
        {
            break;
        }
    }
    
    if (status != CH395Q_CMD_ERR_SUCCESS)
    {
        return CH395Q_ERROR;
    }
    
    return CH395Q_EOK;
}

/**
 * @brief       �Ͽ�Socket��TCP����
 * @param       socket: ָ����Socket
 * @retval      CH395Q_EOK  : Socket��TCP���ӶϿ��ɹ�
 *              CH395Q_ERROR: Socket��TCP���ӶϿ�ʧ��
 */
uint8_t ch395q_cmd_tcp_disconnect_sn(uint8_t socket)
{
    uint8_t status;
    uint8_t timeout = 0;
    
    ch395q_spi_write_cmd(CH395Q_CMD_TCP_DISNCONNECT_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_end_cmd();
    while (1)
    {
        HAL_Delay(5);
        status = ch395q_cmd_get_cmd_status();
        if ((status != CH395Q_CMD_ERR_BUSY) || (timeout++ > 200))
        {
            break;
        }
    }
    
    if (status != CH395Q_CMD_ERR_SUCCESS)
    {
        return CH395Q_ERROR;
    }
    
    return CH395Q_EOK;
}

/**
 * @brief       ��Socket���ͻ�����д����
 * @param       socket: ָ����Socket
 *              buf   : ��д�������
 *              len   : ��д�����ݵĳ���
 * @retval      ��
 */
void ch395q_cmd_write_send_buf_sn(uint8_t socket, uint8_t *buf, uint16_t len)
{
    uint16_t buf_index;
    
    ch395q_spi_write_cmd(CH395Q_CMD_WRITE_SEND_BUF_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_write_dat((uint8_t)len & 0xFF);
    ch395q_spi_write_dat((uint8_t)(len >> 8) & 0xFF);
    for (buf_index=0; buf_index<len; buf_index++)
    {
        ch395q_spi_write_dat(buf[buf_index]);
    }
    ch395q_spi_end_cmd();
}

/**
 * @brief       ��ȡSocket�������ݳ���
 * @param       socket: ָ����Socket
 * @retval      Socket�������ݳ���
 */
uint16_t ch395q_cmd_get_recv_len_sn(uint8_t socket)
{
    uint16_t len;
    
    ch395q_spi_write_cmd(CH395Q_CMD_GET_RECV_LEN_SN);
    ch395q_spi_write_dat(socket);
    len = (uint16_t)ch395q_spi_read_dat() & 0x00FF;
    len |= ((uint16_t)ch395q_spi_read_dat() << 8) & 0xFF00;
    ch395q_spi_end_cmd();
    
    return len;
}

/**
 * @brief       ��Socket���ջ�������������
 * @param       socket: ָ����Socket
 *              len   : ָ���������ݵĳ���
 *              buf   : ���յ�������
 * @retval      Socket�������ݳ���
 */
void ch395q_cmd_read_recv_buf_sn(uint8_t socket, uint16_t len, uint8_t *buf)
{
    uint16_t buf_index;
    
    if (len == 0)
    {
        return;
    }
    
    ch395q_spi_write_cmd(CH395Q_CMD_READ_RECV_BUF_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_write_dat((uint8_t)len & 0xFF);
    ch395q_spi_write_dat((uint8_t)(len >> 8) & 0xFF);
//    HAL_Delay(1)
    for (buf_index=0; buf_index<len; buf_index++)
    {
        buf[buf_index] = ch395q_spi_read_dat();
    }
    ch395q_spi_end_cmd();
}

/**
 * @brief       �ر�Socket
 * @param       socket: ָ����Socket
 * @retval      CH395Q_EOK  : Socket�رճɹ�
 *              CH395Q_ERROR: Socket�ر�ʧ��
 */
uint8_t ch395q_cmd_close_socket_sn(uint8_t socket)
{
    uint8_t status;
    uint8_t timeout = 0;
    
    ch395q_spi_write_cmd(CH395Q_CMD_CLOSE_SOCKET_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_end_cmd();
    while (1)
    {
        HAL_Delay(5);
        status = ch395q_cmd_get_cmd_status();
        if ((status != CH395Q_CMD_ERR_BUSY) || (timeout++ > 200))
        {
            break;
        }
    }
    
    if (status != CH395Q_CMD_ERR_SUCCESS)
    {
        return CH395Q_ERROR;
    }
    
    return CH395Q_EOK;
}

/**
 * @brief       ����Socket��IP����Э���ֶ�
 * @param       socket       : ָ����Socket
 *              ip_proto_code: IP����Э���ֶ�
 * @retval      ��
 */
void ch395q_cmd_set_ipraw_pro_sn(uint8_t socket, uint8_t ip_proto_code)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_IPRAW_PRO_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_write_dat(ip_proto_code);
    ch395q_spi_end_cmd();
}

/**
 * @brief       PINGʹ��
 * @param       enable: PINGʹ�ܱ�־�������ʹ�ܡ�
 * @retval      ��
 */
void ch395q_cmd_ping_enable(uint8_t enable)
{
    ch395q_spi_write_cmd(CH395Q_CMD_PING_ENABLE);
    ch395q_spi_write_dat(enable);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ��ȡMAC��ַ
 * @param       mac_addr: ��ȡ����MAC��ַ
 * @retval      ��
 */
void ch395q_cmd_get_mac_addr(uint8_t *mac_addr)
{
    uint8_t mac_index;
    
    ch395q_spi_write_cmd(CH395Q_CMD_GET_MAC_ADDR);
    for (mac_index=0; mac_index<6; mac_index++)
    {
        mac_addr[mac_index] = ch395q_spi_read_dat();
    }
    ch395q_spi_end_cmd();
}

/**
 * @brief       ������ֹͣ��DHCP
 * @param       enable: DHCPʹ�ܱ�־�������ʹ�ܡ�
 * @retval      CH395Q_EOK  : DHCP������ֹͣ���ɹ�
 *              CH395Q_ERROR: DHCP������ֹͣ��ʧ��
 */
uint8_t ch395q_cmd_dhcp_enable(uint8_t enable)
{
    uint8_t status;
    uint8_t timeout = 0;
    
    ch395q_spi_write_cmd(CH395Q_CMD_DHCP_ENABLE);
    ch395q_spi_write_dat(enable);
    ch395q_spi_end_cmd();
    while (1)
    {
        HAL_Delay(5);
        status = ch395q_cmd_get_cmd_status();
        if ((status != CH395Q_CMD_ERR_BUSY) || (timeout++ > 200))
        {
            break;
        }
    }
    
    if (status != CH395Q_CMD_ERR_SUCCESS)
    {
        return CH395Q_ERROR;
    }
    
    return CH395Q_EOK;
}

/**
 * @brief       ��ȡDHCP״̬
 * @param       ��
 * @retval      CH395Q_EOK  : DHCP�ɹ�
 *              CH395Q_ERROR: DHCPʧ��
 */
uint8_t ch395q_cmd_get_dhcp_status(void)
{
    uint8_t status;
    
    ch395q_spi_write_cmd(CH395Q_CMD_GET_DHCP_STATUS);
    status = ch395q_spi_read_dat();
    ch395q_spi_end_cmd();
    if (status != 0)
    {
        return CH395Q_ERROR;
    }
    
    return CH395Q_EOK;
}

/**
 * @brief       ��ȡIP��MASK��DNS����Ϣ
 * @param       ip_addr  : IP��ַ
 *              gwip_addr: ���ص�ַ
 *              mask_addr: ��������
 *              dns1     : DNS1
 *              dns2     : DNS2
 * @retval      ��
 */
void ch395q_cmd_get_ip_inf(uint8_t *ip_addr, uint8_t *gwip_addr, uint8_t *mask_addr, uint8_t *dns1, uint8_t *dns2)
{
    ch395q_spi_write_cmd(CH395Q_CMD_GET_IP_INF);
    ip_addr[0] = ch395q_spi_read_dat();
    ip_addr[1] = ch395q_spi_read_dat();
    ip_addr[2] = ch395q_spi_read_dat();
    ip_addr[3] = ch395q_spi_read_dat();
    gwip_addr[0] = ch395q_spi_read_dat();
    gwip_addr[1] = ch395q_spi_read_dat();
    gwip_addr[2] = ch395q_spi_read_dat();
    gwip_addr[3] = ch395q_spi_read_dat();
    mask_addr[0] = ch395q_spi_read_dat();
    mask_addr[1] = ch395q_spi_read_dat();
    mask_addr[2] = ch395q_spi_read_dat();
    mask_addr[3] = ch395q_spi_read_dat();
    dns1[0] = ch395q_spi_read_dat();
    dns1[1] = ch395q_spi_read_dat();
    dns1[2] = ch395q_spi_read_dat();
    dns1[3] = ch395q_spi_read_dat();
    dns2[0] = ch395q_spi_read_dat();
    dns2[1] = ch395q_spi_read_dat();
    dns2[2] = ch395q_spi_read_dat();
    dns2[3] = ch395q_spi_read_dat();
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����TCP MSS
 * @param       tcp_mss: TCP MSS
 * @retval      ��
 */
void ch395q_cmd_set_tcp_mss(uint16_t tcp_mss)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_TCP_MSS);
    ch395q_spi_write_dat((uint8_t)tcp_mss & 0xFF);
    ch395q_spi_write_dat((uint8_t)(tcp_mss >> 8) & 0xFF);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����TTLֵ
 * @param       socket: ָ����Socket
 *              ttl   : TTL�����128
 * @retval      ��
 */
void ch395q_cmd_set_ttl(uint8_t socket, uint8_t ttl)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_TTL);
    ch395q_spi_write_dat(socket);
    ch395q_spi_write_dat(ttl);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����Socket���ջ�����
 * @param       socket   : ָ����Socket
 *              blk_start: ��ʼ���ַ
 *              blk_num  : �����
 * @retval      ��
 */
void ch395q_cmd_set_recv_buf(uint8_t socket, uint8_t blk_start, uint8_t blk_num)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_RECV_BUF);
    ch395q_spi_write_dat(socket);
    ch395q_spi_write_dat(blk_start);
    ch395q_spi_write_dat(blk_num);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����Socket���ͻ�����
 * @param       socket   : ָ����Socket
 *              blk_start: ��ʼ���ַ
 *              blk_num  : �����
 * @retval      ��
 */
void ch395q_cmd_set_send_buf(uint8_t socket, uint8_t blk_start, uint8_t blk_num)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_SEND_BUF);
    ch395q_spi_write_dat(socket);
    ch395q_spi_write_dat(blk_start);
    ch395q_spi_write_dat(blk_num);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ���ù��ܲ���
 * @param       para: ���ܲ�������������ܲ�����
 * @retval      ��
 */
void ch395q_cmd_set_fun_para(uint32_t para)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_FUN_PARA);
    ch395q_spi_write_dat((uint8_t)para & 0xFF);
    ch395q_spi_write_dat((uint8_t)(para >> 8) & 0xFF);
    ch395q_spi_write_dat((uint8_t)(para >> 16) & 0xFF);
    ch395q_spi_write_dat((uint8_t)(para >> 24) & 0xFF);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����KEEPLIVE����ʱ��
 * @param       time: KEEPLIVE����ʱ��
 * @retval      ��
 */
void ch395q_cmd_set_keep_live_idle(uint32_t time)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_KEEP_LIVE_IDLE);
    ch395q_spi_write_dat((uint8_t)time & 0xFF);
    ch395q_spi_write_dat((uint8_t)(time >> 8) & 0xFF);
    ch395q_spi_write_dat((uint8_t)(time >> 16) & 0xFF);
    ch395q_spi_write_dat((uint8_t)(time >> 24) & 0xFF);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����KEEPLIVE��ʱʱ��
 * @param       time: KEEPLIVE��ʱʱ��
 * @retval      ��
 */
void ch395q_cmd_set_keep_live_intvl(uint32_t time)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_KEEP_LIVE_INTVL);
    ch395q_spi_write_dat((uint8_t)time & 0xFF);
    ch395q_spi_write_dat((uint8_t)(time >> 8) & 0xFF);
    ch395q_spi_write_dat((uint8_t)(time >> 16) & 0xFF);
    ch395q_spi_write_dat((uint8_t)(time >> 24) & 0xFF);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����KEEPLIVE��ʱ���Դ���
 * @param       cnt: KEEPLIVE��ʱ���Դ���
 * @retval      ��
 */
void ch395q_cmd_set_keep_live_cnt(uint8_t cnt)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_KEEP_LIVE_INTVL);
    ch395q_spi_write_dat(cnt);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����Socket KEEPLIVE
 * @param       cnt   : KEEPLIVE��ʱ���Դ���
 *              enable: Socket KEEPLIVEʹ�ܱ�־�������ʹ�ܡ�
 * @retval      ��
 */
void ch395q_cmd_set_keep_live_sn(uint8_t socket, uint8_t enable)
{
    ch395q_spi_write_cmd(CH395Q_CMD_SET_KEEP_LIVE_SN);
    ch395q_spi_write_dat(socket);
    ch395q_spi_write_dat(enable);
    ch395q_spi_end_cmd();
}

/**
 * @brief       ����EEPROM
 * @param       ��
 * @retval      CH395Q_EOK  : EEPROM�����ɹ�
 *              CH395Q_ERROR: EEPROM����ʧ��
 */
uint8_t ch395q_cmd_eeprom_erase(void)
{
    uint8_t status;
    
    ch395q_spi_write_cmd(CH395Q_CMD_EEPROM_ERASE);
    ch395q_spi_end_cmd();
    while (1)
    {
        HAL_Delay(20);
        status = ch395q_cmd_get_cmd_status();
        if (status != CH395Q_CMD_ERR_BUSY)
        {
            break;
        }
    }
    
    if (status != CH395Q_CMD_ERR_SUCCESS)
    {
        return CH395Q_ERROR;
    }
    
    return CH395Q_EOK;
}

/**
 * @brief       дEEPROM
 * @param       eeprom_addr: EEPROM��ַ
 *              buf        : ��д������
 *              len        : ��д�����ݵĳ���
 * @retval      CH395Q_EOK  : дEEPROM�ɹ�
 *              CH395Q_ERROR: дEEPROMʧ��
 */
uint8_t ch395q_cmd_eeprom_write(uint16_t eeprom_addr, uint8_t *buf, uint8_t len)
{
    uint8_t buf_index;
    uint8_t status;
    
    ch395q_spi_write_cmd(CH395Q_CMD_EEPROM_WRITE);
    ch395q_spi_write_dat((uint8_t)eeprom_addr & 0xFF);
    ch395q_spi_write_dat((uint8_t)(eeprom_addr >> 8) & 0xFF);
    ch395q_spi_write_dat(len);
    ch395q_spi_end_cmd();
    for (buf_index=0; buf_index<len; buf_index++)
    {
        ch395q_spi_write_dat(buf[buf_index]);
    }
    while (1)
    {
        HAL_Delay(20);
        status = ch395q_cmd_get_cmd_status();
        if (status != CH395Q_CMD_ERR_BUSY)
        {
            break;
        }
    }
    
    if (status != CH395Q_CMD_ERR_SUCCESS)
    {
        return CH395Q_ERROR;
    }
    
    return CH395Q_EOK;
}

/**
 * @brief       ��EEPROM
 * @param       eeprom_addr: EEPROM��ַ
 *              buf        : ��ȡ��������
 *              len        : ��ȡ�ĳ���
 * @retval      ��
 */
void ch395q_cmd_eeprom_read(uint16_t eeprom_addr, uint8_t *buf, uint8_t len)
{
    uint8_t buf_index;
    
    ch395q_spi_write_cmd(CH395Q_CMD_EEPROM_READ);
    ch395q_spi_write_dat((uint8_t)eeprom_addr & 0xFF);
    ch395q_spi_write_dat((uint8_t)(eeprom_addr >> 8) & 0xFF);
    ch395q_spi_write_dat(len);
    HAL_Delay(1);
    for (buf_index=0; buf_index<len; buf_index++)
    {
        buf[buf_index] = ch395q_spi_read_dat();
    }
    ch395q_spi_end_cmd();
}

/**
 * @brief       ��GPIO�Ĵ���
 * @param       reg_addr: GPIO�Ĵ�����ַ
 * @retval      GPIO�Ĵ���ֵ
 */
uint8_t ch395q_cmd_read_gpio_reg(uint8_t reg_addr)
{
    uint8_t reg_dat;
    
    ch395q_spi_write_cmd(CH395Q_CMD_READ_GPIO_REG);
    ch395q_spi_write_dat(reg_addr);
    HAL_Delay(1);
    reg_dat = ch395q_spi_read_dat();
    ch395q_spi_end_cmd();
    
    return reg_dat;
}

/**
 * @brief       дGPIO�Ĵ���
 * @param       reg_addr: GPIO�Ĵ�����ַ
 *              reg_dat : GPIO�Ĵ���ֵ
 * @retval      ��
 */
void ch395q_cmd_write_gpio_reg(uint8_t reg_addr, uint8_t reg_dat)
{
    ch395q_spi_write_cmd(CH395Q_CMD_WRITE_GPIO_REG);
    ch395q_spi_write_dat(reg_addr);
    ch395q_spi_write_dat(reg_dat);
    ch395q_spi_end_cmd();
}
