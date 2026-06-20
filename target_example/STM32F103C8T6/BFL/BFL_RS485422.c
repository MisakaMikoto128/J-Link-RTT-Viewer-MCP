/**
 * @file BFL_RS485422.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief RS485/RS422 dual-mode transceiver implementation
 * @version 0.1
 * @date 2025-12-18
 * @last modified 2025-12-18
 *
 * @copyright Copyright (c) 2025 Liu Yuanlin Personal.
 *
 */
#include "BFL_RS485422.h"
#include "HDL_GPIO.h"
#include "main.h"

/**
 * @brief 内部函数: 切换到发送模式 (仅用于RS485模式)
 *
 * @param dev 设备对象指针
 *
 * @note 发送模式: DE=1(高), /RE=1(高)
 *       驱动器输出有效, 接收器关闭(避免回环)
 */
static void BFL_RS485422_EnterTxMode_(BFL_RS485422Dev_t *dev)
{
    if (dev->mode == BFL_RS485422_MODE_RS485) {
        // DE = 1: 使能驱动器
        LL_GPIO_SetOutputPin(dev->deGPIOx, dev->deGPIO_Pin);
        // /RE = 1: 禁用接收器
        LL_GPIO_SetOutputPin(dev->reGPIOx, dev->reGPIO_Pin);
    }
    // RS422模式不需要切换,始终保持DE=1, /RE=0
}

/**
 * @brief 内部函数: 切换到接收模式 (仅用于RS485模式)
 *
 * @param dev 设备对象指针
 *
 * @note 接收模式: DE=0(低), /RE=0(低)
 *       驱动器高阻, 接收器使能
 */
static void BFL_RS485422_EnterRxMode_(BFL_RS485422Dev_t *dev)
{
    if (dev->mode == BFL_RS485422_MODE_RS485) {
        // DE = 0: 禁用驱动器(高阻态)
        LL_GPIO_ResetOutputPin(dev->deGPIOx, dev->deGPIO_Pin);
        // /RE = 0: 使能接收器
        LL_GPIO_ResetOutputPin(dev->reGPIOx, dev->reGPIO_Pin);
    }
    // RS422模式不需要切换,始终保持DE=1, /RE=0
}

/**
 * @brief RS485/RS422设备初始化
 *
 * @param dev 设备对象指针
 */
void BFL_RS485422_Init(BFL_RS485422Dev_t *dev)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能GPIO时钟
    HDL_GPIO_EnablePeripheralCLK(dev->deGPIOx);
    HDL_GPIO_EnablePeripheralCLK(dev->reGPIOx);

    // 初始状态: 接收模式 (DE=0, /RE=0)
    // DE = 0: 禁用驱动器(高阻态)
    LL_GPIO_ResetOutputPin(dev->deGPIOx, dev->deGPIO_Pin);
    // /RE = 0: 使能接收器
    LL_GPIO_ResetOutputPin(dev->reGPIOx, dev->reGPIO_Pin);

    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_DOWN;
    // 配置DE引脚
    GPIO_InitStruct.Pin = dev->deGPIO_Pin;
    LL_GPIO_Init(dev->deGPIOx, &GPIO_InitStruct);
    // 配置/RE引脚
    GPIO_InitStruct.Pin = dev->reGPIO_Pin;
    LL_GPIO_Init(dev->reGPIOx, &GPIO_InitStruct);

    if (dev->mode == BFL_RS485422_MODE_RS485) {
        // ============ RS485半双工模式 ============
        // 确保初始状态为接收模式
        BFL_RS485422_EnterRxMode_(dev);
        // 注册发送完成回调: 自动切换回接收模式
        Uart_SetWriteOverCallback(dev->comID, (UartWriteOverCallback_t)BFL_RS485422_EnterRxMode_, dev);
    } else if (dev->mode == BFL_RS485422_MODE_RS422) {
        // ============ RS422全双工模式 ============
        // 静态配置: DE=1(高), /RE=0(低), 收发器始终使能

        // 确保引脚状态正确
        LL_GPIO_SetOutputPin(dev->deGPIOx, dev->deGPIO_Pin);   // DE = 1
        LL_GPIO_ResetOutputPin(dev->reGPIOx, dev->reGPIO_Pin); // /RE = 0

        // RS422模式不需要发送完成回调
        Uart_SetWriteOverCallback(dev->comID, NULL, NULL);
    } else {
        // ============ 低功耗关断模式 ============
        Uart_SetWriteOverCallback(dev->comID, NULL, NULL);
    }

    // ============ 串口通用配置 ============
    uint32_t wordLen = LL_USART_DATAWIDTH_8B;
    uint32_t parity_ = LL_USART_PARITY_NONE;
    uint32_t stopBit = LL_USART_STOPBITS_1;

    // 数据位配置
    if (dev->dataBit == 7) {
        wordLen = LL_USART_DATAWIDTH_8B;
    } else if (dev->dataBit == 9) {
        wordLen = LL_USART_DATAWIDTH_9B;
    } else {
        wordLen = LL_USART_DATAWIDTH_8B;
    }

    // 校验位配置
    if (dev->parity == 'E') {
        parity_ = LL_USART_PARITY_EVEN;
    } else if (dev->parity == 'O') {
        parity_ = LL_USART_PARITY_ODD;
    } else {
        parity_ = LL_USART_PARITY_NONE;
    }

    // 停止位配置
    switch (dev->stopBit) {
        case 0:
            stopBit = LL_USART_STOPBITS_0_5;
            break;
        case 1:
            stopBit = LL_USART_STOPBITS_1;
            break;
        case 2:
            stopBit = LL_USART_STOPBITS_2;
            break;
        case 3:
            stopBit = LL_USART_STOPBITS_1_5;
            break;
        default:
            stopBit = LL_USART_STOPBITS_1;
            break;
    }

    // 初始化串口
    Uart_Init(dev->comID, dev->baud, wordLen, stopBit, parity_);

    // 标记为已初始化
    dev->__inited = 1;
}

/**
 * @brief RS485/RS422设备写操作
 *
 * @param dev 设备对象指针
 * @param writeBuf 待写数据缓冲区指针
 * @param uLen 需要写的字节数
 * @return size_t >0-实际写出的字节数, 0-未初始化或写失败
 */
size_t BFL_RS485422_Write(BFL_RS485422Dev_t *dev, uint8_t *writeBuf, size_t uLen)
{
    if (dev->comID == COMID_NONE || dev->__inited == 0) {
        return 0;
    }

    // RS485模式需要切换到发送模式
    BFL_RS485422_EnterTxMode_(dev);
    // RS422模式无需切换,直接发送

    size_t ret = Uart_Write(dev->comID, writeBuf, uLen);

    // 注意: RS485模式下,发送完成后会通过回调自动切换回接收模式
    // RS422模式无需切换

    return ret;
}

/**
 * @brief RS485/RS422设备读操作
 *
 * @param dev 设备对象指针
 * @param pBuf 存放读取数据的缓冲区指针
 * @param uLen 本次操作最多能读取的字节数
 * @return size_t >0-实际读取的字节数, 0-没有数据或设备不可用
 */
size_t BFL_RS485422_Read(BFL_RS485422Dev_t *dev, uint8_t *pBuf, size_t uLen)
{
    if (dev->comID == COMID_NONE || dev->__inited == 0) {
        return 0;
    }

    return Uart_Read(dev->comID, pBuf, uLen);
}

/**
 * @brief 获取RS485/RS422设备可读字节数
 *
 * @param dev 设备对象指针
 * @return uint32_t >0-可读字节数, 0-没有数据或设备不可用
 */
uint32_t BFL_RS485422_AvailableBytes(BFL_RS485422Dev_t *dev)
{
    if (dev->comID == COMID_NONE || dev->__inited == 0) {
        return 0;
    }

    return Uart_AvailableBytes(dev->comID);
}
