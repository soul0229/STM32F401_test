#include "demo.h"
#include "ch395q.h"
#include <stdio.h>
#include <string.h>

#define DEMO_SOCKET_PROTO       CH395Q_SOCKET_TCP_SERVER    /* TCP客户端 */
#define DEMO_SOCKET_SOUR_PORT   8080                            /* 本地端口号 */

static uint8_t socket0_send_buf[] = {"This is from CH395Q\r\n"};
static uint8_t socket0_recv_buf[1024];
static uint8_t socket0_send_done = 1;

/**
 * @brief       PHY连接回调
 * @param       phy_status: PHY状态
 * @retval      无
 */
static void demo_phy_conn_cb(uint8_t phy_status)
{
    printf("PHY Connected! ");
    
    switch (phy_status)
    {
        case CH395Q_CMD_PHY_10M_FLL:
        {
            printf("10Mbps Full\r\n");
            break;
        }
        case CH395Q_CMD_PHY_10M_HALF:
        {
            printf("10Mbps Half\r\n");
            break;
        }
        case CH395Q_CMD_PHY_100M_FLL:
        {
            printf("100Mbps Full\r\n");
            break;
        }
        case CH395Q_CMD_PHY_100M_HALF:
        {
            printf("100Mbps Half\r\n");
            break;
        }
        case CH395Q_CMD_PHY_AUTO:
        {
            printf("Auto\r\n");
            break;
        }
        default:
        {
            break;
        }
    }
    
    printf("\r\n");
}

/**
 * @brief       PHY断开回调
 * @param       无
 * @retval      无
 */
static void demo_phy_disconn_cb(void)
{
    printf("PHY Disconnected!\r\n");
    printf("\r\n");
}

/**
 * @brief       DHCP成功回调
 * @param       ip  : IP地址
 *              gwip: 网关IP地址
 *              mask: 子网掩码
 *              dns1: DNS服务器1地址
 *              dns2: DNS服务器2地址
 * @retval      无
 */
static void demo_dhcp_success_cb(uint8_t *ip, uint8_t *gwip, uint8_t *mask, uint8_t *dns1, uint8_t *dns2)
{
    printf("IP: %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
    printf("Gateway: %d.%d.%d.%d\r\n", gwip[0], gwip[1], gwip[2], gwip[3]);
    printf("Mask: %d.%d.%d.%d\r\n", mask[0], mask[1], mask[2], mask[3]);
    printf("DNS1: %d.%d.%d.%d\r\n", dns1[0], dns1[1], dns1[2], dns1[3]);
    printf("DNS2: %d.%d.%d.%d\r\n", dns2[0], dns2[1], dns2[2], dns2[3]);
    printf("\r\n");
}

/**
 * @brief       Socket发送缓冲区空闲回调
 * @param       socket: Socket句柄
 * @retval      无
 */
static void demo_socket_send_buf_free_cb(ch395q_socket_t *socket)
{
    switch (socket->socket_index)
    {
        case CH395Q_SOCKET_0:
        {
            socket0_send_done = 1;
            break;
        }
        default:
        {
            break;
        }
    }
}

/**
 * @brief       Socket接收缓冲区非空回调
 * @param       socket: Socket句柄
 * @retval      无
 */
static void demp_socket_recv_cb(ch395q_socket_t *socket)
{
    uint16_t recv_len;
    
    recv_len = ch395q_cmd_get_recv_len_sn(socket->socket_index);
    if (recv_len != 0)
    {
        if (recv_len > (socket->recv.size - 1))
        {
            recv_len = socket->recv.size;
        }
        ch395q_cmd_read_recv_buf_sn(socket->socket_index, recv_len, socket->recv.buf);
        socket->recv.buf[recv_len] = '\0';
        printf("%s", socket->recv.buf);
    }
}

/**
 * @brief       Socket TCP连接回调
 * @param       socket: Socket句柄
 * @retval      无
 */
static void demo_socket_tcp_conn_cb(ch395q_socket_t *socket)
{
    printf("TCP Connected!\r\n");
    printf("TCP Client IP: %d.%d.%d.%d\r\n", socket->des_ip[0], socket->des_ip[1], socket->des_ip[2], socket->des_ip[3]);
    printf("TCP Client Port: %d\r\n", socket->des_port);
    printf("\r\n");
}

/**
 * @brief       Socket TCP断开回调
 * @param       socket: Socket句柄
 * @retval      无
 */
static void demo_socket_tcp_disconn_cb(ch395q_socket_t *socket)
{
    printf("TCP Disconnected!\r\n");
    printf("\r\n");
}

/**
 * @brief       例程演示入口函数
 * @param       无
 * @retval      无
 */
void demo_run(void)
{
    uint8_t ret;
    ch395q_socket_t socket_config = {0};
    
    /* 初始化CH395Q模块 */
    ret = ch395q_init();
    if (ret != 0)
    {
        printf("CH395Q init failed!\r\n");
        while (1)
        {
            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
            HAL_Delay(200);
        }
    }
    
    /* 配置CH395Q模块网络参数 */
    ch395q_net_config(CH395Q_ENABLE, NULL, NULL, NULL, demo_phy_conn_cb, demo_phy_disconn_cb, demo_dhcp_success_cb);
    
    /* 配置CH395Q模块Socket */
    socket_config.socket_index = CH395Q_SOCKET_0;
    socket_config.enable = CH395Q_ENABLE;
    socket_config.proto = DEMO_SOCKET_PROTO;
    socket_config.sour_port = DEMO_SOCKET_SOUR_PORT;
    socket_config.send.buf = socket0_send_buf;
    socket_config.send.size = sizeof(socket0_send_buf);
    socket_config.recv.buf =  socket0_recv_buf;
    socket_config.recv.size = sizeof(socket0_recv_buf);
    socket_config.send_buf_free_cb = demo_socket_send_buf_free_cb;
    socket_config.recv_cb = demp_socket_recv_cb;
    socket_config.tcp_conn_cb = demo_socket_tcp_conn_cb;
    socket_config.tcp_disconn_cb = demo_socket_tcp_disconn_cb;
    ch395q_socket_config(&socket_config);
    
    while (1)
    {
        ch395q_handler();
        
        if (socket0_send_done == 1)
        {
            socket0_send_done = 0;
            ch395q_cmd_write_send_buf_sn(CH395Q_SOCKET_0, socket0_send_buf, strlen((char *)socket0_send_buf));
        }
    }
}
