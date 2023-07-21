#ifndef __CH395Q_H
#define __CH395Q_H

#include "CH395q_cmd.h"

/* 引脚定义 */
//#define CH395Q_RST_GPIO_PORT            GPIOA
//#define CH395Q_RST_GPIO_PIN             GPIO_PIN_4
//#define CH395Q_RST_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
//#define CH395Q_INT_GPIO_PORT            GPIOA
//#define CH395Q_INT_GPIO_PIN             GPIO_PIN_1
//#define CH395Q_INT_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)

/* IO操作 */
#define CH395Q_RST(x)                   do{ x ?                                                                                     \
                                                HAL_GPIO_WritePin(CH395Q_RST_GPIO_Port, CH395Q_RST_Pin, GPIO_PIN_SET) :    \
                                                HAL_GPIO_WritePin(CH395Q_RST_GPIO_Port, CH395Q_RST_Pin, GPIO_PIN_RESET);   \
                                            }while(0)
#define CH395Q_INT()                    HAL_GPIO_ReadPin(CH395Q_INT_GPIO_Port, CH395Q_INT_Pin)

/* 使能定义 */
#define CH395Q_DISABLE              0   /* 禁用 */
#define CH395Q_ENABLE               1   /* 使能 */

/* CH395Q模块Socket标号定义 */
#define CH395Q_SOCKET_0             0   /* Socket 0 */
#define CH395Q_SOCKET_1             1   /* Socket 1 */
#define CH395Q_SOCKET_2             2   /* Socket 2 */
#define CH395Q_SOCKET_3             3   /* Socket 3 */
#define CH395Q_SOCKET_4             4   /* Socket 4 */
#define CH395Q_SOCKET_5             5   /* Socket 5 */
#define CH395Q_SOCKET_6             6   /* Socket 6 */
#define CH395Q_SOCKET_7             7   /* Socket 7 */

/* CH395Q模块Socket协议类型定义 */
#define CH395Q_SOCKET_UDP           0   /* UDP */
#define CH395Q_SOCKET_TCP_CLIENT    1   /* TCP客户端 */
#define CH395Q_SOCKET_TCP_SERVER    2   /* TCP服务器 */

typedef struct ch395q_socket
{
    uint8_t socket_index;                                       /* Socket标号 */
    uint8_t enable;                                             /* 使能位 */
    uint8_t proto;                                              /* Socket协议 */
    uint8_t des_ip[4];                                          /* 目的IP地址 */
    uint16_t des_port;                                          /* 目的端口 */
    uint16_t sour_port;                                         /* 源端口 */
    struct
    {
        uint8_t *buf;                                           /* 缓冲空间 */
        uint32_t size;                                          /* 缓冲空间大小 */
    } send;                                                     /* 发送缓冲 */
    struct
    {
        uint8_t *buf;                                           /* 缓冲空间 */
        uint32_t size;                                          /* 缓冲空间大小 */
    } recv;                                                     /* 接收缓冲 */
    void (*send_buf_free_cb)(struct ch395q_socket *socket); 		/* 发送缓冲区空闲回调 */
    void (*send_done_cb)(struct ch395q_socket *socket);     		/* 发送成功回调 */
    void (*recv_cb)(struct ch395q_socket *socket);          		/* 接收缓冲区非空回调 */
    void (*tcp_conn_cb)(struct ch395q_socket *socket);      		/* TCP连接回调 */
    void (*tcp_disconn_cb)(struct ch395q_socket *socket);   		/* TCP断开回调 */
    void (*tcp_timeout_cb)(struct ch395q_socket *socket);   		/* TCP超时回调 */
} ch395q_socket_t;

/* 错误代码 */
#define CH395Q_EOK                      0   /* 没有错误 */
#define CH395Q_ERROR                    1   /* 错误 */
#define CH395Q_ETIMEOUT                 2   /* 超时 */
#define CH395Q_EINVAL                   3   /* 参数错误 */

/* 操作函数 */
uint8_t ch395q_init(void);                                                                                                                                                                                                                                          /* ATK-MO395Q模块初始化 */
void ch395q_handler(void);                                                                                                                                                                                                                                          /* ATK-MO395Q模块处理函数 */
uint8_t ch395q_net_config(uint8_t dhcp_enable, uint8_t *ip, uint8_t *gwip, uint8_t *mask, void (*phy_conn_cb)(uint8_t phy_status), void (*phy_disconn_cb)(void), void (*dhcp_success_cb)(uint8_t *ip, uint8_t *gwip, uint8_t *mask, uint8_t *dns1, uint8_t *dns2)); /* ATK-MO395Q模块网络配置 */
uint8_t ch395q_socket_config(ch395q_socket_t *config);                                                                                                                                                                                                          /* ATK-MO395Q模块Socket配置 */

#endif
