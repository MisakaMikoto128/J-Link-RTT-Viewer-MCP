#ifndef _IIC_H
#define _IIC_H
#include "main.h"

#define IIC_SDA_PIN LL_GPIO_PIN_7
#define IIC_SCL_PIN LL_GPIO_PIN_6

#define IIC_SDA_PORT GPIOB
#define IIC_SCL_PORT GPIOB

#define IIC_SDA_PORT_CLK_EN() LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB)
#define IIC_SCL_PORT_CLK_EN() LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB)

#define IIC_SCL_1 LL_GPIO_SetOutputPin(IIC_SCL_PORT, IIC_SCL_PIN)   /* SCL = 1 */
#define IIC_SCL_0 LL_GPIO_ResetOutputPin(IIC_SCL_PORT, IIC_SCL_PIN) /* SCL = 0 */

#define IIC_SDA_1 LL_GPIO_SetOutputPin(IIC_SDA_PORT, IIC_SDA_PIN)   /* SDA = 1 */
#define IIC_SDA_0 LL_GPIO_ResetOutputPin(IIC_SDA_PORT, IIC_SDA_PIN) /* SDA = 0 */

#define IIC_READ_SDA LL_GPIO_IsInputPinSet(IIC_SDA_PORT, IIC_SDA_PIN) /* SDA输入 */

// IIC所有操作函数
void IIC_Init(void);                      // 初始化IIC的IO口
void IIC_Start(void);                     // 发送IIC开始信号
void IIC_Stop(void);                      // 发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);          // IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack); // IIC读取一个字节
uint8_t IIC_Wait_Ack(void);               // IIC等待ACK信号
void IIC_Ack(void);                       // IIC发送ACK信号
void IIC_NAck(void);                      // IIC不发送ACK信号

typedef enum
{
    IIC_1 = 0,
    IIC_NUM,
}IIC_ID_t;

void HDL_IIC_Init(IIC_ID_t iicID, uint32_t speed, uint8_t addrLength,uint16_t ownAddr);

void HDL_IIC_DeInit(IIC_ID_t iicID);

size_t HDL_IIC_Write(IIC_ID_t iicID, uint16_t addr, const uint8_t *data, size_t size);

size_t HDL_IIC_Read(IIC_ID_t iicID, uint16_t addr, uint8_t *buf, size_t size);

size_t HDL_IIC_Mem_Write(IIC_ID_t iicID, uint16_t addr, uint16_t memAddr,uint8_t memAddrLength, const uint8_t *data, size_t size);

size_t HDL_IIC_Mem_Read(IIC_ID_t iicID, uint16_t addr, uint16_t memAddr,uint8_t memAddrLength, uint8_t *buf, size_t size);

size_t HDL_IIC_Write_DMA(IIC_ID_t iicID, uint16_t addr, const uint8_t *data, size_t size);

size_t HDL_IIC_Read_DMA(IIC_ID_t iicID, uint16_t addr, uint8_t *buf, size_t size);

size_t HDL_IIC_Mem_Write_DMA(IIC_ID_t iicID, uint16_t addr, uint16_t memAddr,uint8_t memAddrLength, const uint8_t *data, size_t size);

size_t HDL_IIC_Mem_Read_DMA(IIC_ID_t iicID, uint16_t addr, uint16_t memAddr,uint8_t memAddrLength, uint8_t *buf, size_t size);

I2C_HandleTypeDef *HDL_IIC_GetHALHandle(IIC_ID_t iicID);
#define HDL_IIC_DEFAULT_SPEED 100000
#endif
