#include "HDL_IIC.h"
#include "main.h"
#include "HDL_CPU_Time.h"
#include "log.h"
#include "dma.h"
void IIC_Init()
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  IIC_SDA_PORT_CLK_EN();
  IIC_SCL_PORT_CLK_EN();

  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;

  GPIO_InitStruct.Pin = IIC_SDA_PIN;
  LL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = IIC_SCL_PIN;
  LL_GPIO_Init(IIC_SCL_PORT, &GPIO_InitStruct);
}

void SDA_IN()
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = IIC_SDA_PIN;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);
}

void SDA_OUT()
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = IIC_SDA_PIN;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT; // 推挽输出
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  LL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);
  IIC_SDA_1;
}

// 产生IIC起始信号
void IIC_Start(void)
{
  SDA_OUT();
  IIC_SDA_1;
  IIC_SCL_1;
  HDL_CPU_Time_DelayUs(4);
  IIC_SDA_0;
  HDL_CPU_Time_DelayUs(4);
  IIC_SCL_0;
}
// 产生IIC停止信号
void IIC_Stop(void)
{
  SDA_OUT();
  IIC_SCL_0;
  IIC_SDA_0;
  HDL_CPU_Time_DelayUs(4);
  IIC_SCL_1;
  IIC_SDA_1;
  HDL_CPU_Time_DelayUs(4);
}
// 等待应答信号到来
// 返回值：1，接收应答失败
//         0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
  uint8_t errCount = 0;
  SDA_IN();
  IIC_SDA_1;
  HDL_CPU_Time_DelayUs(2);
  IIC_SCL_1;
  HDL_CPU_Time_DelayUs(2);
  while (IIC_READ_SDA)
  {
    errCount++;
    if (errCount > 250)
    {
      IIC_Stop();
      return 1;
    }
  }
  IIC_SCL_0;

  return 0;
}
// 产生ACK应答
void IIC_Ack(void)
{
  IIC_SCL_0;
  SDA_OUT();
  IIC_SDA_0;
  HDL_CPU_Time_DelayUs(2);
  IIC_SCL_1;
  HDL_CPU_Time_DelayUs(2);
  IIC_SCL_0;
}
// 不产生ACK应答
void IIC_NAck(void)
{
  IIC_SCL_0;
  SDA_OUT();
  IIC_SDA_1;
  HDL_CPU_Time_DelayUs(2);
  IIC_SCL_1;
  HDL_CPU_Time_DelayUs(2);
  IIC_SCL_0;
}
// IIC发送一个字节
// 返回从机有无应答
// 1，有应答
// 0，无应答
void IIC_Send_Byte(uint8_t txd)
{
  uint8_t t;
  SDA_OUT();
  IIC_SCL_0;
  for (t = 0; t < 8; t++)
  {
    if ((txd & 0x80) >> 7)
      IIC_SDA_1;
    else
      IIC_SDA_0;
    txd <<= 1;
    HDL_CPU_Time_DelayUs(2);
    IIC_SCL_1;
    HDL_CPU_Time_DelayUs(2);
    IIC_SCL_0;
    HDL_CPU_Time_DelayUs(2);
  }
}
// 读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t IIC_Read_Byte(unsigned char ack)
{
  unsigned char i, receive = 0;
  SDA_IN(); // SDA设置为输入
  for (i = 0; i < 8; i++)
  {
    IIC_SCL_0;
    HDL_CPU_Time_DelayUs(2);
    IIC_SCL_1;
    receive <<= 1;
    if (IIC_READ_SDA)
      receive++;
    HDL_CPU_Time_DelayUs(1);
  }
  if (!ack)
    IIC_NAck(); // 发送nACK
  else
    IIC_Ack(); // 发送ACK
  return receive;
}

static I2C_HandleTypeDef hi2c1;
static DMA_HandleTypeDef hdma_i2c1_tx;
int i2c_dma_f = 0;
/**
 * @brief IIC初始化。
 *
 * @param iicID IIC编号。
 * @param speed 速度1-100 000Hz。
 * @param addrLength 地址长度，7或10bit。
 * @param ownAddr 本机地址。仅当作为I2C从机时有效。当addrLength = 7bit时，取值范围0-127，当addrLength = 10bit时，取值范围0-1023。
 */
void HDL_IIC_Init(IIC_ID_t iicID, uint32_t speed, uint8_t addrLength, uint16_t ownAddr)
{
  /* USER CODE BEGIN I2C1_Init 0 */
  MX_DMA_Init();
  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = speed;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = ownAddr;
  hi2c1.Init.AddressingMode = addrLength == 7 ? I2C_ADDRESSINGMODE_7BIT : I2C_ADDRESSINGMODE_10BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (i2cHandle->Instance == I2C1)
  {
    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    // strong pull-uphigh to recover from locking in BUSY state
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7; // 此行原有
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;    // GPIO配置为输出
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;       // 高速
    GPIO_InitStruct.Pull = GPIO_PULLUP;            // 强上拉
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_InitStruct.Pin, GPIO_PIN_SET); // 拉高SCL,拉高SDA

    /*Reset I2C*/
    SET_BIT(i2cHandle->Instance->CR1, I2C_CR1_SWRST);   // 复位I2C控制器
    CLEAR_BIT(i2cHandle->Instance->CR1, I2C_CR1_SWRST); // 解除复位（不会自动清除）

    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /* USER CODE BEGIN I2C1_MspInit 1 */

    /* I2C1 DMA Init */
    /* I2C1_TX Init */
    hdma_i2c1_tx.Instance = DMA1_Channel6;
    hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
    hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    if (HAL_DMA_Init(&hdma_i2c1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(i2cHandle, hdmatx, hdma_i2c1_tx);

    /* I2C1 interrupt Init */
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *i2cHandle)
{

  if (i2cHandle->Instance == I2C1)
  {
    /* USER CODE BEGIN I2C1_MspDeInit 0 */

    /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

    /* USER CODE BEGIN I2C1_MspDeInit 1 */
    /* I2C1 DMA DeInit */
    HAL_DMA_DeInit(i2cHandle->hdmatx);

    /* I2C1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
    /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/**
 * @brief This function handles DMA1 channel6 global interrupt.
 */
void DMA1_Channel6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel6_IRQn 0 */

  /* USER CODE END DMA1_Channel6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_i2c1_tx);
  /* USER CODE BEGIN DMA1_Channel6_IRQn 1 */

  /* USER CODE END DMA1_Channel6_IRQn 1 */
}

/**
 * @brief This function handles I2C1 event interrupt.
 */
void I2C1_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_EV_IRQn 0 */

  /* USER CODE END I2C1_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_EV_IRQn 1 */

  /* USER CODE END I2C1_EV_IRQn 1 */
}

/**
 * @brief IIC取消初始化。
 *
 * @param iicID IIC编号。
 */
void HDL_IIC_DeInit(IIC_ID_t iicID)
{
  switch (iicID)
  {
  case IIC_1:
    HAL_I2C_DeInit(&hi2c1);
    break;
  default:
    break;
  }
}

/**
 * @brief IIC发送数据。
 *
 * @param iicID IIC编号。
 * @param addr 目标设备地址。
 * @param data 数据指针。
 * @param size 数据长度。
 * @return size_t 0: 失败，> 1 实际写入的数据长度。
 */
size_t HDL_IIC_Write(IIC_ID_t iicID, uint16_t addr, const uint8_t *data, size_t size)
{
  HAL_StatusTypeDef status = HAL_OK;
  switch (iicID)
  {
  case IIC_1:
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      ;
    status = HAL_I2C_Master_Transmit(&hi2c1, addr, (uint8_t *)data, size, 0xFFFF);
    break;
  default:
    break;
  }
  return status == HAL_OK ? size : 0;
}

/**
 * @brief IIC读取数据。
 *
 * @param iicID IIC编号。
 * @param addr 目标设备地址。
 * @param buf 数据指针。
 * @param size 数据长度。
 * @return size_t 0: 失败，> 1 实际读取的数据长度。
 */
size_t HDL_IIC_Read(IIC_ID_t iicID, uint16_t addr, uint8_t *buf, size_t size)
{
  HAL_StatusTypeDef status = HAL_OK;
  switch (iicID)
  {
  case IIC_1:
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      ;
    status = HAL_I2C_Master_Receive(&hi2c1, addr, buf, size, 0xFFFF);
    break;
  default:
    break;
  }
  return status == HAL_OK ? size : 0;
}

/**
 * @brief Write an amount of data in blocking mode to a specific memory address.
 *
 * @param iicID IIC identifier.
 * @param addr Target device address: The device 7 bits address value
 *         in datasheet must be shifted to the left before calling the interface
 * @param memAddr Internal memory address
 * @param memAddrLength Size of internal memory address.8 or 16
 * @param data Pointer to data buffer
 * @param size Amount of data to be sent
 * @return size_t 0: 失败，> 1 实际写入的数据长度。
 */
size_t HDL_IIC_Mem_Write(IIC_ID_t iicID, uint16_t addr, uint16_t memAddr, uint8_t memAddrLength, const uint8_t *data, size_t size)
{
  HAL_StatusTypeDef status = HAL_OK;
  switch (iicID)
  {
  case IIC_1:
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      ;
    status = HAL_I2C_Mem_Write(&hi2c1, addr, memAddr, memAddrLength == 8 ? I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT, (uint8_t *)data, size, 0xFFFF);
    break;
  default:
    break;
  }
  return status == HAL_OK ? size : 0;
}

/**
 * @brief Read an amount of data in blocking mode from a specific memory address
 *
 * @param iicID IIC identifier.
 * @param addr Target device address: The device 7 bits address value
 *         in datasheet must be shifted to the left before calling the interface
 * @param memAddr Internal memory address
 * @param memAddrLength Size of internal memory address
 * @param buf Pointer to data buffer
 * @param size Amount of data to be sent
 * @return size_t 0: 失败，> 1 实际读取的数据长度。
 */
size_t HDL_IIC_Mem_Read(IIC_ID_t iicID, uint16_t addr, uint16_t memAddr, uint8_t memAddrLength, uint8_t *buf, size_t size)
{
  HAL_StatusTypeDef status = HAL_OK;
  switch (iicID)
  {
  case IIC_1:
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      ;
    status = HAL_I2C_Mem_Write(&hi2c1, addr, memAddr, memAddrLength == 8 ? I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT, buf, size, 0xFFFF);
    break;
  default:
    break;
  }
  return status == HAL_OK ? size : 0;
}

/**
 * @brief IIC发送数据。
 *
 * @param iicID IIC编号。
 * @param addr 目标设备地址。
 * @param data 数据指针。
 * @param size 数据长度。
 * @return size_t 0: 失败，> 1 实际写入的数据长度。
 */
size_t HDL_IIC_Write_DMA(IIC_ID_t iicID, uint16_t addr, const uint8_t *data, size_t size)
{
  HAL_StatusTypeDef status = HAL_OK;
  switch (iicID)
  {
  case IIC_1:
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      ;
    status = HAL_I2C_Master_Transmit_DMA(&hi2c1, addr, (uint8_t *)data, size);
    break;
  default:
    break;
  }
  return status == HAL_OK ? size : 0;
}

/**
 * @brief IIC读取数据。
 *
 * @param iicID IIC编号。
 * @param addr 目标设备地址。
 * @param buf 数据指针。
 * @param size 数据长度。
 * @return size_t 0: 失败，> 1 实际读取的数据长度。
 */
size_t HDL_IIC_Read_DMA(IIC_ID_t iicID, uint16_t addr, uint8_t *buf, size_t size)
{
  HAL_StatusTypeDef status = HAL_OK;
  switch (iicID)
  {
  case IIC_1:
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      ;
    status = HAL_I2C_Master_Receive(&hi2c1, addr, buf, size, 0xFFFF);
    break;
  default:
    break;
  }
  return status == HAL_OK ? size : 0;
}

/**
 * @brief Write an amount of data in blocking mode to a specific memory address.
 *
 * @param iicID IIC identifier.
 * @param addr Target device address: The device 7 bits address value
 *         in datasheet must be shifted to the left before calling the interface
 * @param memAddr Internal memory address
 * @param memAddrLength Size of internal memory address.8 or 16
 * @param data Pointer to data buffer
 * @param size Amount of data to be sent
 * @return size_t 0: 失败，> 1 实际写入的数据长度。
 */
size_t HDL_IIC_Mem_Write_DMA(IIC_ID_t iicID, uint16_t addr, uint16_t memAddr, uint8_t memAddrLength, const uint8_t *data, size_t size)
{
  HAL_StatusTypeDef status = HAL_OK;
  switch (iicID)
  {
  case IIC_1:
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      ;
    status = HAL_I2C_Mem_Write_DMA(&hi2c1, addr, memAddr, memAddrLength == 8 ? I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT, (uint8_t *)data, size);
    break;
  default:
    break;
  }
  return status == HAL_OK ? size : 0;
}

/**
 * @brief Read an amount of data in blocking mode from a specific memory address
 *
 * @param iicID IIC identifier.
 * @param addr Target device address: The device 7 bits address value
 *         in datasheet must be shifted to the left before calling the interface
 * @param memAddr Internal memory address
 * @param memAddrLength Size of internal memory address
 * @param buf Pointer to data buffer
 * @param size Amount of data to be sent
 * @return size_t 0: 失败，> 1 实际读取的数据长度。
 */
size_t HDL_IIC_Mem_Read_DMA(IIC_ID_t iicID, uint16_t addr, uint16_t memAddr, uint8_t memAddrLength, uint8_t *buf, size_t size)
{
  HAL_StatusTypeDef status = HAL_OK;
  switch (iicID)
  {
  case IIC_1:
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      ;
    status = HAL_I2C_Mem_Write_DMA(&hi2c1, addr, memAddr, memAddrLength == 8 ? I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT, buf, size);
    break;
  default:
    break;
  }
  return status == HAL_OK ? size : 0;
}

I2C_HandleTypeDef *HDL_IIC_GetHALHandle(IIC_ID_t iicID)
{
  switch (iicID)
  {
  case IIC_1:
    return &hi2c1;
  default:
    return NULL;
  }
}