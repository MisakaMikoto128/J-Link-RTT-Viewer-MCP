/**
 * @file BFL_RS422.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-16
 * @last modified 2023-01-16
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#ifndef BFL_RS422_H
#define BFL_RS422_H

#include "HDL_GPIO.h"
#include "HDL_Uart.h"
typedef struct tagBFL_RS422Dev_t
{
    GPIO_TypeDef *DE_GPIO_Port;  // 方向引脚所在的GPIO端口
    uint32_t DE_GPIO_Pin;    // 方向引脚
    GPIO_TypeDef *RE_GPIO_Port; // 方向引脚所在的GPIO端口
    uint32_t RE_GPIO_Pin;   // 方向引脚
    COMID_t comID;                 // 串口号
    uint32_t baud;                 // 通信波特率，eg. 9600, 19200, 57600, 115200, etc.
    char parity;                   // 'N' for none, 'E' for even, 'O' for odd
    uint8_t dataBit;               // 串口通信的位宽，可以是7, 8 and 9.
    uint8_t stopBit;               // 停止位，可以是0, 1, 2.对于支持1.5停止位的串口，可以是3.
    uint8_t __inited;               // 是否已经初始化
} BFL_RS422Dev_t;

/**
 * @brief RS422外设初始化方法。
 *
 * @param dev RS422外设对象。
 */
void BFL_RS422_Init(BFL_RS422Dev_t *dev);

/**
 * @brief RS422外设写操作
 *
 * @param dev RS422外设对象
 * @param writeBuf 存放待写数据缓存区的指针
 * @param uLen 需要写多少个字节
 * @return uint32_t >0-写出去实际字节数，0-未初始化，写失败
 */
size_t BFL_RS422_Write(BFL_RS422Dev_t *dev, uint8_t *writeBuf, size_t uLen);

/**
 * @brief RS422外设读操作
 *
 * @param dev RS422外设对象
 * @param pBuf 存放读取数据的缓存区的指针
 * @param uiLen 本次操作最多能读取的字节数
 * @return uint32_t >0-实际读取的字节数，0-没有数据或者RS422外设不可用
 */
size_t BFL_RS422_Read(BFL_RS422Dev_t *dev, uint8_t *pBuf, size_t uLen);

/**
 * @brief RS422外设可读字节数。
 *
 *
 * @param dev RS422外设对象。
 * @return uint32_t >0-可读字节数，0-没有数据或者RS422外设不可用。
 */

uint32_t BFL_RS422_AvailableBytes(BFL_RS422Dev_t *dev);

extern BFL_RS422Dev_t *RS4221;

#endif //! BFL_RS422_H