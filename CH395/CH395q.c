#include "CH395q.h"
#include "CH395q_cmd.h"
#include "CH395q_spi.h"
#include <string.h>
/* �������ڲ���������������� */
#define CH395Q_TEST_DAT (0x65)

static struct
{
    uint8_t version;                            /* �汾��Ϣ */
    uint8_t phy_status;                         /* PHY״̬ */
    struct
    {
        uint8_t dhcp_enable;                    /* DHCP���ñ�־ */
        uint8_t ip[4];                          /* IP��ַ */
        uint8_t gwip[4];                        /* ����IP��ַ */
        uint8_t mask[4];                        /* �������� */
    } net_config;                               /* ����������Ϣ */
    uint8_t dhcp_status;                        /* DHCP״̬ */
    uint8_t static_status;                      /* ��̬�����������״̬ */
    struct
    {
        uint8_t ip[4];                          /* IP��ַ */
        uint8_t gwip[4];                        /* ����IP��ַ */
        uint8_t mask[4];                        /* �������� */
        uint8_t dns1[4];                        /* DNS������1��ַ */
        uint8_t dns2[4];                        /* DNS������2��ַ */
    } net_info;                                 /* ������Ϣ */
    struct
    {
        uint8_t enable_status;                  /* ʹ����Ϣ */
        ch395q_socket_t config;             		/* ������Ϣ */
    } socket[8];                                /* Socket״̬ */
    void (*phy_conn_cb)(uint8_t phy_status);    /* PHY���ӻص� */
    void (*phy_disconn_cb)(void);               /* PHY�Ͽ��ص� */
    void (*dhcp_success_cb)(uint8_t *ip, uint8_t *gwip, uint8_t *mask, uint8_t *dns1, uint8_t *dns2);   /* DHCP�ɹ��ص� */
} g_ch395q_sta = {0};

/**
 * @brief       CH395Qģ��Ӳ����ʼ��
 * @param       ��
 * @retval      ��
 */
static void ch395q_hw_init(void)
{
//    GPIO_InitTypeDef gpio_init_struct = {0};
//    
//    /* ʹ��ʱ�� */
//    CH395Q_RST_GPIO_CLK_ENABLE();
//    CH395Q_INT_GPIO_CLK_ENABLE();
//    
//    /* ��ʼ��RST���� */
//    gpio_init_struct.Pin    = CH395Q_RST_GPIO_PIN;
//    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;
//    gpio_init_struct.Pull   = GPIO_PULLUP;
//    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(CH395Q_RST_GPIO_PORT, &gpio_init_struct);
//    
//    /* ��ʼ��INT���� */
//    gpio_init_struct.Pin    = CH395Q_INT_GPIO_PIN;
//    gpio_init_struct.Mode   = GPIO_MODE_INPUT;
//    gpio_init_struct.Pull   = GPIO_PULLUP;
//    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(CH395Q_INT_GPIO_PORT, &gpio_init_struct);
    
    CH395Q_RST(1);
}

/**
 * @brief       CH395Qģ��Ӳ����λ
 * @note        RST���������ͷŸ�λ����ȴ�����35����CH395Qģ�������������
 * @param       ��
 * @retval      ��
 */
static void ch395q_hw_reset(void)
{
    CH395Q_RST(0);
    HAL_Delay(50);
    CH395Q_RST(1);
    HAL_Delay(50);
}

/**
 * @brief       CH395Qģ�������λ
 * @param       ��
 * @retval      ��
 */
static void ch395q_sw_reset(void)
{
    ch395q_cmd_reset_all();
    HAL_Delay(100);
}

/**
 * @brief       CH395Qģ��ȫ�ֱ�����ʼ��
 * @param       ��
 * @retval      ��
 */
static void ch395q_parameter_init(void)
{
    uint8_t socket_index;
    
    g_ch395q_sta.version = ch395q_cmd_get_ic_version();
    g_ch395q_sta.phy_status = ch395q_cmd_get_phy_status();
    g_ch395q_sta.dhcp_status = CH395Q_ERROR;
    g_ch395q_sta.static_status = CH395Q_ERROR;
    for (socket_index=CH395Q_SOCKET_0; socket_index<=CH395Q_SOCKET_7; socket_index++)
    {
        g_ch395q_sta.socket[socket_index].enable_status = CH395Q_DISABLE;
        g_ch395q_sta.socket[socket_index].config.enable = CH395Q_DISABLE;
    }
}

/**
 * @brief       CH395Qģ��KeepLive��ʼ��
 * @param       ��
 * @retval      ��
 */
static void ch395q_keeplive_init(void)
{
    ch395q_cmd_set_keep_live_cnt(200);
    ch395q_cmd_set_keep_live_idle(15000);
    ch395q_cmd_set_keep_live_intvl(20000);
}

/**
 * @brief       CH395Qģ��Socket�жϴ�����
 * @param       socket_index: Socket���
 * @retval      ��
 */
static void ch395q_socket_interrupt_handler(uint8_t socket_index)
{
    uint8_t int_status;
    
    /* ��ȡSocket�ж�״̬ */
    int_status = ch395q_cmd_get_int_status_sn(socket_index);
    
    /* ���ͻ��������У��Ѿ��������������ͻ�����д�������ˣ� */
    if (int_status & CH395Q_CMD_SOCKET_INT_STAT_SENBUF_FREE)
    {
        if (g_ch395q_sta.socket[socket_index].config.send_buf_free_cb != NULL)
        {
            g_ch395q_sta.socket[socket_index].config.send_buf_free_cb(&g_ch395q_sta.socket[socket_index].config);
        }
    }
    
    /* ���ͳɹ� */
    if (int_status & CH395Q_CMD_SOCKET_INT_STAT_SEND_OK)
    {
        if (g_ch395q_sta.socket[socket_index].config.send_done_cb != NULL)
        {
            g_ch395q_sta.socket[socket_index].config.send_done_cb(&g_ch395q_sta.socket[socket_index].config);
        }
    }
    
    /* ���ջ������ǿ� */
    if (int_status & CH395Q_CMD_SOCKET_INT_STAT_RECV)
    {
        if (g_ch395q_sta.socket[socket_index].config.recv_cb != NULL)
        {
            g_ch395q_sta.socket[socket_index].config.recv_cb(&g_ch395q_sta.socket[socket_index].config);
        }
    }
    
    /* TCP���ӣ�����TCPģʽ����Ч�� */
    if (int_status & CH395Q_CMD_SOCKET_INT_STAT_CONNECT)
    {
        /* �����TCP�����������ȡ�ͻ��˵�IP�Ͷ˿� */
        if (g_ch395q_sta.socket[socket_index].config.proto == CH395Q_SOCKET_TCP_SERVER)
        {
            ch395q_cmd_get_remot_ipp_sn(socket_index, g_ch395q_sta.socket[socket_index].config.des_ip, &g_ch395q_sta.socket[socket_index].config.des_port);
        }
        
        if (g_ch395q_sta.socket[socket_index].config.tcp_conn_cb != NULL)
        {
            g_ch395q_sta.socket[socket_index].config.tcp_conn_cb(&g_ch395q_sta.socket[socket_index].config);
        }
    }
    
    /* TCP�Ͽ�������TCPģʽ����Ч�� */
    if (int_status & CH395Q_CMD_SOCKET_INT_STAT_DISCONNECT)
    {
        /* TCP�Ͽ���Ĭ�ϻ��Զ��ر�Socket */
        g_ch395q_sta.socket[socket_index].enable_status = CH395Q_DISABLE;
        
        if (g_ch395q_sta.socket[socket_index].config.tcp_disconn_cb != NULL)
        {
            g_ch395q_sta.socket[socket_index].config.tcp_disconn_cb(&g_ch395q_sta.socket[socket_index].config);
        }
    }
    
    /* ��ʱ������TCPģʽ����Ч�� */
    if (int_status & CH395Q_CMD_SOCKET_INT_STAT_TIM_OUT)
    {
        /* ��ʱ��Ĭ�ϻ��Զ��ر�Socket */
        g_ch395q_sta.socket[socket_index].enable_status = CH395Q_DISABLE;
        
        if (g_ch395q_sta.socket[socket_index].config.tcp_timeout_cb != NULL)
        {
            g_ch395q_sta.socket[socket_index].config.tcp_timeout_cb(&g_ch395q_sta.socket[socket_index].config);
        }
    }
}

/**
 * @brief       ����CH395Qģ��ȫ���ж�
 * @param       ��
 * @retval      ��
 */
static void ch395q_global_interrupt_handler(void)
{
    uint16_t int_status;
    
    /* ��ȡȫ���ж�״̬ */
    int_status = ch395q_cmd_get_glob_int_status_all();
    
    /* ���ɴ��ж� */
    if (int_status & CH395Q_CMD_GINT_STAT_UNREACH)
    {
        
    }
    
    /* IP��ͻ */
    if (int_status & CH395Q_CMD_GINT_STAT_IP_CONFLI)
    {
        
    }
    
    /* PHY״̬�ı��ж� */
    if (int_status & CH395Q_CMD_GINT_STAT_PHY_CHANGE)
    {
        /* ��ȡPHY״̬ */
        g_ch395q_sta.phy_status = ch395q_cmd_get_phy_status();
    }
    
    /* DHCP��PPPOE�ж� */
    if (int_status & CH395Q_CMD_GINT_STAT_DHCP_PPPOE)
    {
        /* ��ȡDHCP״̬ */
        g_ch395q_sta.dhcp_status = ch395q_cmd_get_dhcp_status();
    }
    
    /* Socket0�ж� */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK0)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_0);
    }
    
    /* Socket1�ж� */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK1)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_1);
    }
    
    /* Socket2�ж� */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK2)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_2);
    }
    
    /* Socket3�ж� */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK3)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_3);
    }
    
    /* Socket4�ж� */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK4)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_4);
    }
    
    /* Socket5�ж� */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK5)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_5);
    }
    
    /* Socket6�ж� */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK6)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_6);
    }
    
    /* Socket7�ж� */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK7)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_7);
    }
}

/**
 * @brief       CH395Qģ���ʼ��
 * @param       ��
 * @retval      CH395Q_EOK  : CH395Qģ���ʼ���ɹ�
 * @retval      CH395Q_ERROR: CH395Qģ���ʼ��ʧ��
 */
uint8_t ch395q_init(void)
{
    uint8_t res;
    uint8_t ret;
    
    ch395q_hw_init();                                   /* Ӳ����ʼ�� */
    ch395q_hw_reset();                                  /* Ӳ����λ */
//    ch395q_spi_init();                                  /* SPI�ӿڳ�ʼ�� */
    res = ch395q_cmd_check_exist(CH395Q_TEST_DAT);  		/* ͨѶ�ӿڲ��� */
    if (res != (uint8_t)(~CH395Q_TEST_DAT))
    {
        return CH395Q_ERROR;
    }
    ch395q_sw_reset();                                  /* �����λ */
    ch395q_keeplive_init();                             /* KeepLive��ʼ�� */
    ret = ch395q_cmd_init_ch395();                      /* ��ʼ��CH395QоƬ */
    if (ret != CH395Q_EOK)
    {
        return CH395Q_ERROR;
    }
    ch395q_parameter_init();                            /* ȫ�ֱ�����ʼ�� */
    
    return CH395Q_EOK;
}

/**
 * @brief       CH395Qģ�鴦����
 * @note        �ú���ʱCH395Qģ��Ĵ���������Ҫ����ʱ���ã����ü��Խ��Խ��
 * @param       ��
 * @retval      ��
 */
void ch395q_handler(void)
{
    static uint8_t last_phy_status = CH395Q_CMD_PHY_DISCONN;
    static uint8_t last_dhcp_status = CH395Q_ERROR;
    static uint8_t last_static_status = CH395Q_ERROR;
    uint8_t socket_index;
    uint8_t ret;
    
    if (CH395Q_INT() == 0)
    {
        /* ����CH395Qģ��ȫ���ж� */
        ch395q_global_interrupt_handler();
    }
    
    /* PHY������ */
    if (g_ch395q_sta.phy_status != CH395Q_CMD_PHY_DISCONN)
    {
        /* PHY�Ͽ����״����ӵĳ�ʼ�� */
        if (last_phy_status == CH395Q_CMD_PHY_DISCONN)
        {
            if (g_ch395q_sta.phy_conn_cb != NULL)
            {
                g_ch395q_sta.phy_conn_cb(g_ch395q_sta.phy_status);
            }
            
            if (g_ch395q_sta.net_config.dhcp_enable == CH395Q_ENABLE)
            {
                /* ʹ��DHCP */
                ch395q_cmd_dhcp_enable(CH395Q_CMD_ENABLE);
            }
            else
            {
                /* �ر�DHCP�����þ�̬������� */
                ch395q_cmd_dhcp_enable(CH395Q_CMD_DISABLE);
                ch395q_cmd_set_ip_addr(g_ch395q_sta.net_config.ip);
                ch395q_cmd_set_gwip_addr(g_ch395q_sta.net_config.gwip);
                ch395q_cmd_set_mask_addr(g_ch395q_sta.net_config.mask);
                ch395q_cmd_init_ch395();
                HAL_Delay(10);
                g_ch395q_sta.static_status = CH395Q_EOK;
                ch395q_cmd_get_ip_inf(g_ch395q_sta.net_info.ip, g_ch395q_sta.net_info.gwip, g_ch395q_sta.net_info.mask, g_ch395q_sta.net_info.dns1, g_ch395q_sta.net_info.dns2);
            }
        }
    }
    else
    {
        /* PHY�Ͽ�����״β��� */
        if (last_phy_status != CH395Q_CMD_PHY_DISCONN)
        {
            if (g_ch395q_sta.phy_disconn_cb != NULL)
            {
                g_ch395q_sta.phy_disconn_cb();
            }
            
            /* ��λ�����³�ʼ��CH395QоƬ */
            ch395q_sw_reset();
            ch395q_cmd_init_ch395();
            
            /* ��ʼ��DHCP״̬Ϊʧ��״̬ */
            g_ch395q_sta.dhcp_status = CH395Q_ERROR;
            /* ���û�����ù���̬��Ϣ */
            last_static_status = CH395Q_ERROR;
        }
    }
    
    /* DHCPʹ�ܲ��ɹ� */
    if ((g_ch395q_sta.net_config.dhcp_enable == CH395Q_ENABLE) && (g_ch395q_sta.dhcp_status == CH395Q_EOK))
    {
        /* DHCP�״γɹ� */
        if (last_dhcp_status == CH395Q_ERROR)
        {
            /* �ر�DHCP */
            ch395q_cmd_dhcp_enable(CH395Q_CMD_DISABLE);
            /* ��ȡDHCP��̬�����IP����Ϣ */
            ch395q_cmd_get_ip_inf(g_ch395q_sta.net_info.ip, g_ch395q_sta.net_info.gwip, g_ch395q_sta.net_info.mask, g_ch395q_sta.net_info.dns1, g_ch395q_sta.net_info.dns2);
            if (g_ch395q_sta.dhcp_success_cb != NULL)
            {
                g_ch395q_sta.dhcp_success_cb(g_ch395q_sta.net_info.ip, g_ch395q_sta.net_info.gwip, g_ch395q_sta.net_info.mask, g_ch395q_sta.net_info.dns1, g_ch395q_sta.net_info.dns2);
            }
            /* ���û�����ù���̬��Ϣ */
            last_static_status = CH395Q_ERROR;
        }
    }
    /* û��ʹ��DHCP */
    else if (g_ch395q_sta.net_config.dhcp_enable == CH395Q_DISABLE)
    {
        /* ������һ�ξ�̬��Ϣ */
        if (last_static_status == CH395Q_ERROR)
        {
            /* �ر�DHCP�����þ�̬������� */
            ch395q_cmd_dhcp_enable(CH395Q_CMD_DISABLE);
            ch395q_cmd_set_ip_addr(g_ch395q_sta.net_config.ip);
            ch395q_cmd_set_gwip_addr(g_ch395q_sta.net_config.gwip);
            ch395q_cmd_set_mask_addr(g_ch395q_sta.net_config.mask);
            ch395q_cmd_init_ch395();
            HAL_Delay(10);
            g_ch395q_sta.static_status = CH395Q_EOK;
            ch395q_cmd_get_ip_inf(g_ch395q_sta.net_info.ip, g_ch395q_sta.net_info.gwip, g_ch395q_sta.net_info.mask, g_ch395q_sta.net_info.dns1, g_ch395q_sta.net_info.dns2);
            last_static_status = CH395Q_EOK;
        }
    }
    
    /* PHY���ӳɹ���DHCP�������ɹ����䵽IP��DHCP�رվ�̬������IP��������Socket */
    if ((g_ch395q_sta.phy_status != CH395Q_CMD_PHY_DISCONN) && (((g_ch395q_sta.net_config.dhcp_enable == CH395Q_ENABLE) && (g_ch395q_sta.dhcp_status == CH395Q_EOK)) || ((g_ch395q_sta.net_config.dhcp_enable == CH395Q_DISABLE) && (g_ch395q_sta.static_status == CH395Q_EOK))))
    {
        /* ��������Socket */
        for (socket_index=CH395Q_SOCKET_0; socket_index<=CH395Q_SOCKET_7; socket_index++)
        {
            /* Socket����Ϊ���� */
            if (g_ch395q_sta.socket[socket_index].config.enable == CH395Q_ENABLE)
            {
                /* Socketδ�����������ò�����Socket */
                if (g_ch395q_sta.socket[socket_index].enable_status == CH395Q_DISABLE)
                {
                    /* ���ݲ�ͬЭ�飬���ò�����Socket */
                    switch (g_ch395q_sta.socket[socket_index].config.proto)
                    {
                        case CH395Q_SOCKET_UDP:
                        {
                            ch395q_cmd_set_proto_type_sn(socket_index, CH395Q_CMD_PROTO_TYPE_UDP);
                            ch395q_cmd_set_ip_addr_sn(socket_index, g_ch395q_sta.socket[socket_index].config.des_ip);
                            ch395q_cmd_set_des_port_sn(socket_index, g_ch395q_sta.socket[socket_index].config.des_port);
                            ch395q_cmd_set_sour_port_sn(socket_index, g_ch395q_sta.socket[socket_index].config.sour_port);
                            ret = ch395q_cmd_open_socket_sn(socket_index);
                            break;
                        }
                        case CH395Q_SOCKET_TCP_CLIENT:
                        {
                            ch395q_cmd_set_proto_type_sn(socket_index, CH395Q_CMD_PROTO_TYPE_TCP);
                            ch395q_cmd_set_ip_addr_sn(socket_index, g_ch395q_sta.socket[socket_index].config.des_ip);
                            ch395q_cmd_set_des_port_sn(socket_index, g_ch395q_sta.socket[socket_index].config.des_port);
                            ch395q_cmd_set_sour_port_sn(socket_index, g_ch395q_sta.socket[socket_index].config.sour_port);
                            ret = ch395q_cmd_open_socket_sn(socket_index);
                            ret += ch395q_cmd_tcp_connect_sn(socket_index);
                            break;
                        }
                        case CH395Q_SOCKET_TCP_SERVER:
                        {
                            ch395q_cmd_set_proto_type_sn(socket_index, CH395Q_CMD_PROTO_TYPE_TCP);
                            ch395q_cmd_set_sour_port_sn(socket_index, g_ch395q_sta.socket[socket_index].config.sour_port);
                            ret = ch395q_cmd_open_socket_sn(socket_index);
                            ret += ch395q_cmd_tcp_listen_sn(socket_index);
                        }
                        default:
                        {
                            break;
                        }
                    }
                    
                    if (ret == CH395Q_EOK)
                    {
                        g_ch395q_sta.socket[socket_index].enable_status = CH395Q_ENABLE;
                        if (g_ch395q_sta.socket[socket_index].config.proto == CH395Q_SOCKET_TCP_CLIENT)
                        {
                            /* ʹ��TCP Clietnʱ������Socket���KeepLive���� */
                            ch395q_cmd_set_keep_live_sn(socket_index, CH395Q_CMD_ENABLE);
                            /* ��Socket���������TTLֵ */
                            ch395q_cmd_set_ttl(socket_index, 40);
                        }
                    }
                }
            }
            else
            {
                /* Socket�ѿ�����ر�Socket */
                if (g_ch395q_sta.socket[socket_index].enable_status == CH395Q_ENABLE)
                {
                    ret = ch395q_cmd_close_socket_sn(socket_index);
                    if (ret == CH395Q_EOK)
                    {
                        g_ch395q_sta.socket[socket_index].enable_status = CH395Q_DISABLE;
                    }
                }
            }
        }
    }
    else
    {
        /* ��������Socket */
        for (socket_index=CH395Q_SOCKET_0; socket_index<=CH395Q_SOCKET_7; socket_index++)
        {
            /* �ر������ѿ�����Socket */
            if (g_ch395q_sta.socket[socket_index].enable_status == CH395Q_ENABLE)
            {
                ch395q_cmd_close_socket_sn(socket_index);
                g_ch395q_sta.socket[socket_index].enable_status = CH395Q_DISABLE;
            }
        }
    }
    
    last_phy_status = g_ch395q_sta.phy_status;
    last_dhcp_status = g_ch395q_sta.dhcp_status;
}

/**
 * @brief       CH395Qģ����������
 * @param       dhcp_enable    : �Ƿ�ʹ��DHCP
 *              ip             : Ҫ���õľ�̬IP��ַ��DHCPʧ��ʱʹ�ã�
 *              gwip           : Ҫ���õ�����IP��ַ��DHCPʧ��ʱʹ�ã�
 *              mask           : Ҫ���õ��������루DHCPʧ��ʱʹ�ã�
 *              phy_conn_cb    : PHY���ӻص�����
 *              phy_disconn_cb : PHY�Ͽ��ص�����
 *              dhcp_success_cb: DHCP�ɹ��ص�����
 * @retval      CH395Q_EOK   : ���ò�������
 *              CH395Q_EINVAL: ���ò�������
 */
uint8_t ch395q_net_config(uint8_t dhcp_enable, uint8_t *ip, uint8_t *gwip, uint8_t *mask, void (*phy_conn_cb)(uint8_t phy_status), void (*phy_disconn_cb)(void), void (*dhcp_success_cb)(uint8_t *ip, uint8_t *gwip, uint8_t *mask, uint8_t *dns1, uint8_t *dns2))
{
    if ((dhcp_enable == CH395Q_DISABLE) && ((ip == NULL) || (gwip == NULL) || (mask == NULL)))
    {
        return CH395Q_EINVAL;
    }
    
    g_ch395q_sta.net_config.dhcp_enable = dhcp_enable;
    if (ip != NULL)
    {
        memcpy(g_ch395q_sta.net_config.ip, ip, sizeof(g_ch395q_sta.net_config.ip));
    }
    if (gwip != NULL)
    {
        memcpy(g_ch395q_sta.net_config.gwip, gwip, sizeof(g_ch395q_sta.net_config.gwip));
    }
    if (mask != NULL)
    {
        memcpy(g_ch395q_sta.net_config.mask, mask, sizeof(g_ch395q_sta.net_config.mask));
    }
    
    if (phy_conn_cb != NULL)
    {
        g_ch395q_sta.phy_conn_cb = phy_conn_cb;
    }
    
    if (phy_disconn_cb != NULL)
    {
        g_ch395q_sta.phy_disconn_cb = phy_disconn_cb;
    }
    
    if (dhcp_success_cb != NULL)
    {
        g_ch395q_sta.dhcp_success_cb = dhcp_success_cb;
    }
    
    return CH395Q_EOK;
}

/**
 * @brief       CH395Qģ��Socket����
 * @param       config: Socket������Ϣ
 * @retval      CH395Q_EOK   : Socket���óɹ�
 *              CH395Q_EINVAL: ���ò�������
 */
uint8_t ch395q_socket_config(ch395q_socket_t *config)
{
    if (config == NULL)
    {
        return CH395Q_EINVAL;
    }
    
    memcpy(&g_ch395q_sta.socket[config->socket_index].config, config, sizeof(ch395q_socket_t));
    
    return CH395Q_EOK;
}
