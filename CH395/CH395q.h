#ifndef __CH395Q_H
#define __CH395Q_H

#include "CH395q_cmd.h"

/* ���Ŷ��� */
//#define CH395Q_RST_GPIO_PORT            GPIOA
//#define CH395Q_RST_GPIO_PIN             GPIO_PIN_4
//#define CH395Q_RST_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
//#define CH395Q_INT_GPIO_PORT            GPIOA
//#define CH395Q_INT_GPIO_PIN             GPIO_PIN_1
//#define CH395Q_INT_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)

/* IO���� */
#define CH395Q_RST(x)                   do{ x ?                                                                                     \
                                                HAL_GPIO_WritePin(CH395Q_RST_GPIO_Port, CH395Q_RST_Pin, GPIO_PIN_SET) :    \
                                                HAL_GPIO_WritePin(CH395Q_RST_GPIO_Port, CH395Q_RST_Pin, GPIO_PIN_RESET);   \
                                            }while(0)
#define CH395Q_INT()                    HAL_GPIO_ReadPin(CH395Q_INT_GPIO_Port, CH395Q_INT_Pin)

/* ʹ�ܶ��� */
#define CH395Q_DISABLE              0   /* ���� */
#define CH395Q_ENABLE               1   /* ʹ�� */

/* CH395Qģ��Socket��Ŷ��� */
#define CH395Q_SOCKET_0             0   /* Socket 0 */
#define CH395Q_SOCKET_1             1   /* Socket 1 */
#define CH395Q_SOCKET_2             2   /* Socket 2 */
#define CH395Q_SOCKET_3             3   /* Socket 3 */
#define CH395Q_SOCKET_4             4   /* Socket 4 */
#define CH395Q_SOCKET_5             5   /* Socket 5 */
#define CH395Q_SOCKET_6             6   /* Socket 6 */
#define CH395Q_SOCKET_7             7   /* Socket 7 */

/* CH395Qģ��SocketЭ�����Ͷ��� */
#define CH395Q_SOCKET_UDP           0   /* UDP */
#define CH395Q_SOCKET_TCP_CLIENT    1   /* TCP�ͻ��� */
#define CH395Q_SOCKET_TCP_SERVER    2   /* TCP������ */

typedef struct ch395q_socket
{
    uint8_t socket_index;                                       /* Socket��� */
    uint8_t enable;                                             /* ʹ��λ */
    uint8_t proto;                                              /* SocketЭ�� */
    uint8_t des_ip[4];                                          /* Ŀ��IP��ַ */
    uint16_t des_port;                                          /* Ŀ�Ķ˿� */
    uint16_t sour_port;                                         /* Դ�˿� */
    struct
    {
        uint8_t *buf;                                           /* ����ռ� */
        uint32_t size;                                          /* ����ռ��С */
    } send;                                                     /* ���ͻ��� */
    struct
    {
        uint8_t *buf;                                           /* ����ռ� */
        uint32_t size;                                          /* ����ռ��С */
    } recv;                                                     /* ���ջ��� */
    void (*send_buf_free_cb)(struct ch395q_socket *socket); 		/* ���ͻ��������лص� */
    void (*send_done_cb)(struct ch395q_socket *socket);     		/* ���ͳɹ��ص� */
    void (*recv_cb)(struct ch395q_socket *socket);          		/* ���ջ������ǿջص� */
    void (*tcp_conn_cb)(struct ch395q_socket *socket);      		/* TCP���ӻص� */
    void (*tcp_disconn_cb)(struct ch395q_socket *socket);   		/* TCP�Ͽ��ص� */
    void (*tcp_timeout_cb)(struct ch395q_socket *socket);   		/* TCP��ʱ�ص� */
} ch395q_socket_t;

/* ������� */
#define CH395Q_EOK                      0   /* û�д��� */
#define CH395Q_ERROR                    1   /* ���� */
#define CH395Q_ETIMEOUT                 2   /* ��ʱ */
#define CH395Q_EINVAL                   3   /* �������� */

/* �������� */
uint8_t ch395q_init(void);                                                                                                                                                                                                                                          /* ATK-MO395Qģ���ʼ�� */
void ch395q_handler(void);                                                                                                                                                                                                                                          /* ATK-MO395Qģ�鴦���� */
uint8_t ch395q_net_config(uint8_t dhcp_enable, uint8_t *ip, uint8_t *gwip, uint8_t *mask, void (*phy_conn_cb)(uint8_t phy_status), void (*phy_disconn_cb)(void), void (*dhcp_success_cb)(uint8_t *ip, uint8_t *gwip, uint8_t *mask, uint8_t *dns1, uint8_t *dns2)); /* ATK-MO395Qģ���������� */
uint8_t ch395q_socket_config(ch395q_socket_t *config);                                                                                                                                                                                                          /* ATK-MO395Qģ��Socket���� */

#endif
