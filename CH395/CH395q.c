#include "CH395q.h"
#include "CH395q_cmd.h"
#include "CH395q_spi.h"
#include <string.h>
/* 定义用于测试命令的任意数据 */
#define CH395Q_TEST_DAT (0x65)

static struct
{
    uint8_t version;                            /* 版本信息 */
    uint8_t phy_status;                         /* PHY状态 */
    struct
    {
        uint8_t dhcp_enable;                    /* DHCP启用标志 */
        uint8_t ip[4];                          /* IP地址 */
        uint8_t gwip[4];                        /* 网关IP地址 */
        uint8_t mask[4];                        /* 子网掩码 */
    } net_config;                               /* 网络配置信息 */
    uint8_t dhcp_status;                        /* DHCP状态 */
    uint8_t static_status;                      /* 静态配置网络参数状态 */
    struct
    {
        uint8_t ip[4];                          /* IP地址 */
        uint8_t gwip[4];                        /* 网关IP地址 */
        uint8_t mask[4];                        /* 子网掩码 */
        uint8_t dns1[4];                        /* DNS服务器1地址 */
        uint8_t dns2[4];                        /* DNS服务器2地址 */
    } net_info;                                 /* 网络信息 */
    struct
    {
        uint8_t enable_status;                  /* 使能信息 */
        ch395q_socket_t config;             		/* 配置信息 */
    } socket[8];                                /* Socket状态 */
    void (*phy_conn_cb)(uint8_t phy_status);    /* PHY连接回调 */
    void (*phy_disconn_cb)(void);               /* PHY断开回调 */
    void (*dhcp_success_cb)(uint8_t *ip, uint8_t *gwip, uint8_t *mask, uint8_t *dns1, uint8_t *dns2);   /* DHCP成功回调 */
} g_ch395q_sta = {0};

/**
 * @brief       CH395Q模块硬件初始化
 * @param       无
 * @retval      无
 */
static void ch395q_hw_init(void)
{
//    GPIO_InitTypeDef gpio_init_struct = {0};
//    
//    /* 使能时钟 */
//    CH395Q_RST_GPIO_CLK_ENABLE();
//    CH395Q_INT_GPIO_CLK_ENABLE();
//    
//    /* 初始化RST引脚 */
//    gpio_init_struct.Pin    = CH395Q_RST_GPIO_PIN;
//    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;
//    gpio_init_struct.Pull   = GPIO_PULLUP;
//    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(CH395Q_RST_GPIO_PORT, &gpio_init_struct);
//    
//    /* 初始化INT引脚 */
//    gpio_init_struct.Pin    = CH395Q_INT_GPIO_PIN;
//    gpio_init_struct.Mode   = GPIO_MODE_INPUT;
//    gpio_init_struct.Pull   = GPIO_PULLUP;
//    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(CH395Q_INT_GPIO_PORT, &gpio_init_struct);
    
    CH395Q_RST(1);
}

/**
 * @brief       CH395Q模块硬件复位
 * @note        RST引脚拉高释放复位后，需等待至少35毫秒CH395Q模块才能正常工作
 * @param       无
 * @retval      无
 */
static void ch395q_hw_reset(void)
{
    CH395Q_RST(0);
    HAL_Delay(50);
    CH395Q_RST(1);
    HAL_Delay(50);
}

/**
 * @brief       CH395Q模块软件复位
 * @param       无
 * @retval      无
 */
static void ch395q_sw_reset(void)
{
    ch395q_cmd_reset_all();
    HAL_Delay(100);
}

/**
 * @brief       CH395Q模块全局变量初始化
 * @param       无
 * @retval      无
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
 * @brief       CH395Q模块KeepLive初始化
 * @param       无
 * @retval      无
 */
static void ch395q_keeplive_init(void)
{
    ch395q_cmd_set_keep_live_cnt(200);
    ch395q_cmd_set_keep_live_idle(15000);
    ch395q_cmd_set_keep_live_intvl(20000);
}

/**
 * @brief       CH395Q模块Socket中断处理函数
 * @param       socket_index: Socket标号
 * @retval      无
 */
static void ch395q_socket_interrupt_handler(uint8_t socket_index)
{
    uint8_t int_status;
    
    /* 获取Socket中断状态 */
    int_status = ch395q_cmd_get_int_status_sn(socket_index);
    
    /* 发送缓冲区空闲（已经继续可以往发送缓冲区写入数据了） */
    if (int_status & CH395Q_CMD_SOCKET_INT_STAT_SENBUF_FREE)
    {
        if (g_ch395q_sta.socket[socket_index].config.send_buf_free_cb != NULL)
        {
            g_ch395q_sta.socket[socket_index].config.send_buf_free_cb(&g_ch395q_sta.socket[socket_index].config);
        }
    }
    
    /* 发送成功 */
    if (int_status & CH395Q_CMD_SOCKET_INT_STAT_SEND_OK)
    {
        if (g_ch395q_sta.socket[socket_index].config.send_done_cb != NULL)
        {
            g_ch395q_sta.socket[socket_index].config.send_done_cb(&g_ch395q_sta.socket[socket_index].config);
        }
    }
    
    /* 接收缓冲区非空 */
    if (int_status & CH395Q_CMD_SOCKET_INT_STAT_RECV)
    {
        if (g_ch395q_sta.socket[socket_index].config.recv_cb != NULL)
        {
            g_ch395q_sta.socket[socket_index].config.recv_cb(&g_ch395q_sta.socket[socket_index].config);
        }
    }
    
    /* TCP连接（仅在TCP模式下有效） */
    if (int_status & CH395Q_CMD_SOCKET_INT_STAT_CONNECT)
    {
        /* 如果是TCP服务器，则获取客户端的IP和端口 */
        if (g_ch395q_sta.socket[socket_index].config.proto == CH395Q_SOCKET_TCP_SERVER)
        {
            ch395q_cmd_get_remot_ipp_sn(socket_index, g_ch395q_sta.socket[socket_index].config.des_ip, &g_ch395q_sta.socket[socket_index].config.des_port);
        }
        
        if (g_ch395q_sta.socket[socket_index].config.tcp_conn_cb != NULL)
        {
            g_ch395q_sta.socket[socket_index].config.tcp_conn_cb(&g_ch395q_sta.socket[socket_index].config);
        }
    }
    
    /* TCP断开（仅在TCP模式下有效） */
    if (int_status & CH395Q_CMD_SOCKET_INT_STAT_DISCONNECT)
    {
        /* TCP断开后，默认会自动关闭Socket */
        g_ch395q_sta.socket[socket_index].enable_status = CH395Q_DISABLE;
        
        if (g_ch395q_sta.socket[socket_index].config.tcp_disconn_cb != NULL)
        {
            g_ch395q_sta.socket[socket_index].config.tcp_disconn_cb(&g_ch395q_sta.socket[socket_index].config);
        }
    }
    
    /* 超时（仅在TCP模式下有效） */
    if (int_status & CH395Q_CMD_SOCKET_INT_STAT_TIM_OUT)
    {
        /* 超时后，默认会自动关闭Socket */
        g_ch395q_sta.socket[socket_index].enable_status = CH395Q_DISABLE;
        
        if (g_ch395q_sta.socket[socket_index].config.tcp_timeout_cb != NULL)
        {
            g_ch395q_sta.socket[socket_index].config.tcp_timeout_cb(&g_ch395q_sta.socket[socket_index].config);
        }
    }
}

/**
 * @brief       处理CH395Q模块全局中断
 * @param       无
 * @retval      无
 */
static void ch395q_global_interrupt_handler(void)
{
    uint16_t int_status;
    
    /* 获取全局中断状态 */
    int_status = ch395q_cmd_get_glob_int_status_all();
    
    /* 不可达中断 */
    if (int_status & CH395Q_CMD_GINT_STAT_UNREACH)
    {
        
    }
    
    /* IP冲突 */
    if (int_status & CH395Q_CMD_GINT_STAT_IP_CONFLI)
    {
        
    }
    
    /* PHY状态改变中断 */
    if (int_status & CH395Q_CMD_GINT_STAT_PHY_CHANGE)
    {
        /* 获取PHY状态 */
        g_ch395q_sta.phy_status = ch395q_cmd_get_phy_status();
    }
    
    /* DHCP和PPPOE中断 */
    if (int_status & CH395Q_CMD_GINT_STAT_DHCP_PPPOE)
    {
        /* 获取DHCP状态 */
        g_ch395q_sta.dhcp_status = ch395q_cmd_get_dhcp_status();
    }
    
    /* Socket0中断 */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK0)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_0);
    }
    
    /* Socket1中断 */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK1)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_1);
    }
    
    /* Socket2中断 */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK2)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_2);
    }
    
    /* Socket3中断 */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK3)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_3);
    }
    
    /* Socket4中断 */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK4)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_4);
    }
    
    /* Socket5中断 */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK5)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_5);
    }
    
    /* Socket6中断 */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK6)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_6);
    }
    
    /* Socket7中断 */
    if (int_status & CH395Q_CMD_GINT_STAT_SOCK7)
    {
        ch395q_socket_interrupt_handler(CH395Q_SOCKET_7);
    }
}

/**
 * @brief       CH395Q模块初始化
 * @param       无
 * @retval      CH395Q_EOK  : CH395Q模块初始化成功
 * @retval      CH395Q_ERROR: CH395Q模块初始化失败
 */
uint8_t ch395q_init(void)
{
    uint8_t res;
    uint8_t ret;
    
    ch395q_hw_init();                                   /* 硬件初始化 */
    ch395q_hw_reset();                                  /* 硬件复位 */
//    ch395q_spi_init();                                  /* SPI接口初始化 */
    res = ch395q_cmd_check_exist(CH395Q_TEST_DAT);  		/* 通讯接口测试 */
    if (res != (uint8_t)(~CH395Q_TEST_DAT))
    {
        return CH395Q_ERROR;
    }
    ch395q_sw_reset();                                  /* 软件复位 */
    ch395q_keeplive_init();                             /* KeepLive初始化 */
    ret = ch395q_cmd_init_ch395();                      /* 初始化CH395Q芯片 */
    if (ret != CH395Q_EOK)
    {
        return CH395Q_ERROR;
    }
    ch395q_parameter_init();                            /* 全局变量初始化 */
    
    return CH395Q_EOK;
}

/**
 * @brief       CH395Q模块处理函数
 * @note        该函数时CH395Q模块的处理函数，需要被定时调用，调用间隔越短越好
 * @param       无
 * @retval      无
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
        /* 处理CH395Q模块全局中断 */
        ch395q_global_interrupt_handler();
    }
    
    /* PHY已连接 */
    if (g_ch395q_sta.phy_status != CH395Q_CMD_PHY_DISCONN)
    {
        /* PHY断开后首次连接的初始化 */
        if (last_phy_status == CH395Q_CMD_PHY_DISCONN)
        {
            if (g_ch395q_sta.phy_conn_cb != NULL)
            {
                g_ch395q_sta.phy_conn_cb(g_ch395q_sta.phy_status);
            }
            
            if (g_ch395q_sta.net_config.dhcp_enable == CH395Q_ENABLE)
            {
                /* 使能DHCP */
                ch395q_cmd_dhcp_enable(CH395Q_CMD_ENABLE);
            }
            else
            {
                /* 关闭DHCP并配置静态网络参数 */
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
        /* PHY断开后的首次操作 */
        if (last_phy_status != CH395Q_CMD_PHY_DISCONN)
        {
            if (g_ch395q_sta.phy_disconn_cb != NULL)
            {
                g_ch395q_sta.phy_disconn_cb();
            }
            
            /* 复位并重新初始化CH395Q芯片 */
            ch395q_sw_reset();
            ch395q_cmd_init_ch395();
            
            /* 初始化DHCP状态为失败状态 */
            g_ch395q_sta.dhcp_status = CH395Q_ERROR;
            /* 标记没有配置过静态信息 */
            last_static_status = CH395Q_ERROR;
        }
    }
    
    /* DHCP使能并成功 */
    if ((g_ch395q_sta.net_config.dhcp_enable == CH395Q_ENABLE) && (g_ch395q_sta.dhcp_status == CH395Q_EOK))
    {
        /* DHCP首次成功 */
        if (last_dhcp_status == CH395Q_ERROR)
        {
            /* 关闭DHCP */
            ch395q_cmd_dhcp_enable(CH395Q_CMD_DISABLE);
            /* 获取DHCP动态分配的IP等信息 */
            ch395q_cmd_get_ip_inf(g_ch395q_sta.net_info.ip, g_ch395q_sta.net_info.gwip, g_ch395q_sta.net_info.mask, g_ch395q_sta.net_info.dns1, g_ch395q_sta.net_info.dns2);
            if (g_ch395q_sta.dhcp_success_cb != NULL)
            {
                g_ch395q_sta.dhcp_success_cb(g_ch395q_sta.net_info.ip, g_ch395q_sta.net_info.gwip, g_ch395q_sta.net_info.mask, g_ch395q_sta.net_info.dns1, g_ch395q_sta.net_info.dns2);
            }
            /* 标记没有配置过静态信息 */
            last_static_status = CH395Q_ERROR;
        }
    }
    /* 没有使能DHCP */
    else if (g_ch395q_sta.net_config.dhcp_enable == CH395Q_DISABLE)
    {
        /* 仅配置一次静态信息 */
        if (last_static_status == CH395Q_ERROR)
        {
            /* 关闭DHCP并配置静态网络参数 */
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
    
    /* PHY连接成功且DHCP开启并成功分配到IP或DHCP关闭静态设置了IP，才配置Socket */
    if ((g_ch395q_sta.phy_status != CH395Q_CMD_PHY_DISCONN) && (((g_ch395q_sta.net_config.dhcp_enable == CH395Q_ENABLE) && (g_ch395q_sta.dhcp_status == CH395Q_EOK)) || ((g_ch395q_sta.net_config.dhcp_enable == CH395Q_DISABLE) && (g_ch395q_sta.static_status == CH395Q_EOK))))
    {
        /* 遍历所有Socket */
        for (socket_index=CH395Q_SOCKET_0; socket_index<=CH395Q_SOCKET_7; socket_index++)
        {
            /* Socket配置为开启 */
            if (g_ch395q_sta.socket[socket_index].config.enable == CH395Q_ENABLE)
            {
                /* Socket未开启，则配置并开启Socket */
                if (g_ch395q_sta.socket[socket_index].enable_status == CH395Q_DISABLE)
                {
                    /* 根据不同协议，配置并开启Socket */
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
                            /* 使用TCP Clietn时，创建Socket后打开KeepLive功能 */
                            ch395q_cmd_set_keep_live_sn(socket_index, CH395Q_CMD_ENABLE);
                            /* 打开Socket后才能设置TTL值 */
                            ch395q_cmd_set_ttl(socket_index, 40);
                        }
                    }
                }
            }
            else
            {
                /* Socket已开，则关闭Socket */
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
        /* 遍历所有Socket */
        for (socket_index=CH395Q_SOCKET_0; socket_index<=CH395Q_SOCKET_7; socket_index++)
        {
            /* 关闭所有已开启的Socket */
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
 * @brief       CH395Q模块网络配置
 * @param       dhcp_enable    : 是否使能DHCP
 *              ip             : 要设置的静态IP地址（DHCP失败时使用）
 *              gwip           : 要设置的网关IP地址（DHCP失败时使用）
 *              mask           : 要设置的子网掩码（DHCP失败时使用）
 *              phy_conn_cb    : PHY连接回调函数
 *              phy_disconn_cb : PHY断开回调函数
 *              dhcp_success_cb: DHCP成功回调函数
 * @retval      CH395Q_EOK   : 配置参数无误
 *              CH395Q_EINVAL: 配置参数有误
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
 * @brief       CH395Q模块Socket配置
 * @param       config: Socket配置信息
 * @retval      CH395Q_EOK   : Socket配置成功
 *              CH395Q_EINVAL: 配置参数有误
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
