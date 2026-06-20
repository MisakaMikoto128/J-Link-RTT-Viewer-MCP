/**
 * @file BFL_RS485.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-16
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */
#include "BFL_RS485.h"
#include "HDL_GPIO.h"
#include "main.h"

/**
 * @brief RS485外设获取总线方法。获取总线后，RS485外设可以发送数据。
 *
 * @param dev
 */
void BFL_RS485_GetBus_(BFL_RS485Dev_t *dev)
{
    LL_GPIO_SetOutputPin(dev->DE_GPIO_Port, dev->DE_GPIO_Pin);
    if (dev->RE_GPIO_Port != NULL)
    {
        LL_GPIO_SetOutputPin(dev->RE_GPIO_Port, dev->RE_GPIO_Pin);
    }
}

/**
 * @brief RS485外设释放总线方法。释放总线后，RS485外设才能接收数据。
 *
 * @param dev
 */
void BFL_RS485_ReleaseBus_(BFL_RS485Dev_t *dev)
{
    LL_GPIO_ResetOutputPin(dev->DE_GPIO_Port, dev->DE_GPIO_Pin);
    if (dev->RE_GPIO_Port != NULL)
    {
        LL_GPIO_ResetOutputPin(dev->RE_GPIO_Port, dev->RE_GPIO_Pin);
    }
}

/**
 * @brief RS485外设初始化方法。
 *
 * @param dev RS485外设对象。
 */
void BFL_RS485_Init(BFL_RS485Dev_t *dev)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    // GPIO初始化
    HDL_GPIO_EnablePeripheralCLK(dev->DE_GPIO_Port);

    BFL_RS485_ReleaseBus_(dev);
    /**/
    GPIO_InitStruct.Pin = dev->DE_GPIO_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(dev->DE_GPIO_Port, &GPIO_InitStruct);
    // 初始化时释放RS485总线
    BFL_RS485_ReleaseBus_(dev);

    if (dev->RE_GPIO_Port != NULL)
    {
        HDL_GPIO_EnablePeripheralCLK(dev->RE_GPIO_Port);
        BFL_RS485_ReleaseBus_(dev);
        /**/
        GPIO_InitStruct.Pin = dev->RE_GPIO_Pin;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
        LL_GPIO_Init(dev->RE_GPIO_Port, &GPIO_InitStruct);
        // 初始化时释放RS485总线
        BFL_RS485_ReleaseBus_(dev);
    }

    uint32_t wordLen = LL_USART_DATAWIDTH_8B;
    uint32_t parity_ = LL_USART_PARITY_NONE;
    uint32_t stopBit = LL_USART_STOPBITS_1;
    wordLen = dev->dataBit == 7 ? LL_USART_DATAWIDTH_8B : LL_USART_DATAWIDTH_8B;
    wordLen = dev->dataBit == 9 ? LL_USART_DATAWIDTH_9B : LL_USART_DATAWIDTH_8B;
    parity_ = dev->parity == 'E' ? LL_USART_PARITY_EVEN : LL_USART_PARITY_NONE;
    parity_ = dev->parity == 'O' ? LL_USART_PARITY_ODD : LL_USART_PARITY_NONE;

    switch (dev->stopBit)
    {
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
    Uart_SetWriteOverCallback(dev->comID, (UartWriteOverCallback_t)BFL_RS485_ReleaseBus_, dev);
    Uart_Init(dev->comID, dev->baud, wordLen, stopBit, parity_);
    dev->__inited = 1;
}

/**
 * @brief RS485外设写操作
 *
 * @param dev RS485外设对象
 * @param writeBuf 存放待写数据缓存区的指针
 * @param uLen 需要写多少个字节
 * @return uint32_t >0-写出去实际字节数，0-未初始化，写失败
 */
size_t BFL_RS485_Write(BFL_RS485Dev_t *dev, uint8_t *writeBuf, size_t uLen)
{
    if (dev->comID == COMID_NONE)
    {
        return 0;
    }
    BFL_RS485_GetBus_(dev);
    size_t ret = Uart_Write(dev->comID, writeBuf, uLen);
    return ret;
}

/**
 * @brief RS485外设读操作
 *
 * @param dev RS485外设对象
 * @param pBuf 存放读取数据的缓存区的指针
 * @param uiLen 本次操作最多能读取的字节数
 * @return uint32_t >0-实际读取的字节数，0-没有数据或者RS485外设不可用
 */
size_t BFL_RS485_Read(BFL_RS485Dev_t *dev, uint8_t *pBuf, size_t uLen)
{
    if (dev->comID == COMID_NONE)
    {
        return 0;
    }
    size_t ret = Uart_Read(dev->comID, pBuf, uLen);
    return ret;
}

/**
 * @brief RS485外设可读字节数。
 *
 *
 * @param dev RS485外设对象。
 * @return uint32_t >0-可读字节数，0-没有数据或者RS485外设不可用。
 */

uint32_t BFL_RS485_AvailableBytes(BFL_RS485Dev_t *dev)
{
    if (dev->comID == COMID_NONE)
    {
        return 0;
    }
    return Uart_AvailableBytes(dev->comID);
}