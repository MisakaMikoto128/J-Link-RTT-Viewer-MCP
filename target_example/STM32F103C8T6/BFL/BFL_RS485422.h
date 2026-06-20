/**
 * @file BFL_RS485422.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief RS485/RS422 dual-mode transceiver abstraction layer
 * @version 0.1
 * @date 2025-12-18
 * @last modified 2025-12-18
 *
 * @copyright Copyright (c) 2025 Liu Yuanlin Personal.
 *
 */
#ifndef BFL_RS485422_H
#define BFL_RS485422_H

#include "HDL_GPIO.h"
#include "HDL_Uart.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief RS485/RS422工作模式枚举
 */
typedef enum
{
    BFL_RS485422_MODE_RS485 = 0,  // RS485半双工模式
    BFL_RS485422_MODE_RS422 = 1,   // RS422全双工模式
    BFL_RS485422_MODE_DISABLED = 2 // 低功耗关断
} BFL_RS485422_Mode_t;

/**
 * @brief RS485/RS422设备对象结构体
 */
typedef struct tagBFL_RS485422Dev_t
{
    // 工作模式
    BFL_RS485422_Mode_t mode;      // 工作模式: RS485 或 RS422

    // DE (Driver Enable) 引脚配置
    GPIO_TypeDef *deGPIOx;         // DE引脚所在的GPIO端口
    uint32_t deGPIO_Pin;           // DE引脚编号

    // /RE (Receiver Enable, active low) 引脚配置
    GPIO_TypeDef *reGPIOx;         // /RE引脚所在的GPIO端口
    uint32_t reGPIO_Pin;           // /RE引脚编号

    // 串口配置
    COMID_t comID;                 // 串口号
    uint32_t baud;                 // 通信波特率，eg. 9600, 19200, 57600, 115200, etc.
    char parity;                   // 'N' for none, 'E' for even, 'O' for odd
    uint8_t dataBit;               // 串口通信的位宽，可以是7, 8 and 9.
    uint8_t stopBit;               // 停止位，可以是0, 1, 2.对于支持1.5停止位的串口，可以是3.

    // 内部状态
    uint8_t __inited;               // 是否已经初始化
} BFL_RS485422Dev_t;

/**
 * @brief RS485/RS422设备初始化
 *
 * @param dev 设备对象指针
 *
 * @note RS485模式: DE和/RE动态控制,初始化为接收状态(DE=0, /RE=0)
 *       RS422模式: DE固定为高(1), /RE固定为低(0),收发器始终使能
 */
void BFL_RS485422_Init(BFL_RS485422Dev_t *dev);

/**
 * @brief RS485/RS422设备写操作
 *
 * @param dev 设备对象指针
 * @param writeBuf 待写数据缓冲区指针
 * @param uLen 需要写的字节数
 * @return size_t >0-实际写出的字节数, 0-未初始化或写失败
 *
 * @note RS485模式: 写操作前自动切换到发送模式(DE=1, /RE=1)
 *                  发送完成后通过回调自动切换回接收模式
 *       RS422模式: 直接写入,无需切换模式(全双工)
 */
size_t BFL_RS485422_Write(BFL_RS485422Dev_t *dev, uint8_t *writeBuf, size_t uLen);

/**
 * @brief RS485/RS422设备读操作
 *
 * @param dev 设备对象指针
 * @param pBuf 存放读取数据的缓冲区指针
 * @param uLen 本次操作最多能读取的字节数
 * @return size_t >0-实际读取的字节数, 0-没有数据或设备不可用
 */
size_t BFL_RS485422_Read(BFL_RS485422Dev_t *dev, uint8_t *pBuf, size_t uLen);

/**
 * @brief 获取RS485/RS422设备可读字节数
 *
 * @param dev 设备对象指针
 * @return uint32_t >0-可读字节数, 0-没有数据或设备不可用
 */
uint32_t BFL_RS485422_AvailableBytes(BFL_RS485422Dev_t *dev);

#endif // BFL_RS485422_H
