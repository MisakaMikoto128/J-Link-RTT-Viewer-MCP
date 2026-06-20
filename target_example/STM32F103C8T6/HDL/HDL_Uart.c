/**
 * @file HDL_Uart.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-11
 * @brief This file is for STM32F103C6Tx CPU.
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */
#include "HDL_Uart.h"
#include "Queue.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "HDL_CPU.h"

/**************************
 *
 *
优先级：
NVIC_PRIORITYGROUP_4 :
	4 bits for pre-emption priority,
	0 bit  for subpriority

COM map:
COM1 <---> USART1
COM2 <---> USART2


PIN Map:
USART1
	PA9   ------> USART1_TX
	PA10   ------> USART1_RX
USART2
	PA2   ------> USART2_RX
	PA3   ------> USART2_TX

这个串口库的发送全部启用了FIFO，大小为8，并且启用了TX FIFO empty中断。
如果对字符实时性要求高的场合FIFO可能会产生不好的影响，需要自己修改。
**************************/

// 串口设备抽象
COM_Dev_t COM1_Dev = {.com = COM1, .inited = 0, .write_over_callback = NULL, .receive_char_callback = NULL};
COM_Dev_t COM2_Dev = {.com = COM2, .inited = 0, .write_over_callback = NULL, .receive_char_callback = NULL};
COM_Dev_t COM3_Dev = {.com = COM3, .inited = 0, .write_over_callback = NULL, .receive_char_callback = NULL};

// 串口1相关变量
#define COM1_RX_BUF_SIZE 256
static QUEUE8_t m_QueueCom1Rx = {0};
static uint8_t m_Com1RxBuf[COM1_RX_BUF_SIZE] = {0};

#define COM1_TX_BUF_SIZE 300
static QUEUE8_t m_QueueCom1Tx = {0};
static uint8_t m_Com1TxBuf[COM1_TX_BUF_SIZE] = {0};

// 串口2相关变量
#define COM2_RX_BUF_SIZE 150
static QUEUE8_t m_QueueCom2Rx = {0};
static uint8_t m_Com2RxBuf[COM2_RX_BUF_SIZE] = {0};

#define COM2_TX_BUF_SIZE 50
static QUEUE8_t m_QueueCom2Tx = {0};
static uint8_t m_Com2TxBuf[COM2_TX_BUF_SIZE] = {0};

// 串口3相关变量
#define COM3_RX_BUF_SIZE 30
static QUEUE8_t m_QueueCom3Rx = {0};
static uint8_t m_Com3RxBuf[COM3_RX_BUF_SIZE] = {0};

#define COM3_TX_BUF_SIZE 20
static QUEUE8_t m_QueueCom3Tx = {0};
static uint8_t m_Com3TxBuf[COM3_TX_BUF_SIZE] = {0};

/**
 * @brief 串口初始化
 *
 * @param comx 串口号
 * @param bound 波特率
 * @param wordLen 数据宽度 LL_USART_DATAWIDTH_7B,LL_USART_DATAWIDTH_8B,LL_USART_DATAWIDTH_9B
 * @param stopBit 停止位个数LL_USART_STOPBITS_1,LL_USART_STOPBITS_2
 * @param parity 奇偶校验位LL_USART_PARITY_NONE LL_USART_PARITY_EVEN LL_USART_PARITY_ODD
 * @note 如果是串口是LPUART，那么推荐使用的初始化参数名字如下：
 * wordLen-LL_LPUART_DATAWIDTH_7B,LL_LPUART_DATAWIDTH_8B,LL_LPUART_DATAWIDTH_9B
 * stopBit-LL_LPUART_STOPBITS_1,LL_LPUART_STOPBITS_2
 * parity-LL_LPUART_PARITY_NONE,LL_LPUART_PARITY_EVEN,LL_LPUART_PARITY_ODD
 */
void Uart_Init(COMID_t comid, uint32_t bound, uint32_t wordLen, uint32_t stopBit,
			   uint32_t parity)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	LL_USART_InitTypeDef USART_InitStruct = {0};
	switch (comid)
	{
	case COM1:
	{
		// 创建接收数据缓冲区
		QUEUE_PacketCreate(&m_QueueCom1Rx, m_Com1RxBuf, sizeof(m_Com1RxBuf));
		// 创建发送数据缓冲区
		QUEUE_PacketCreate(&m_QueueCom1Tx, m_Com1TxBuf, sizeof(m_Com1TxBuf));

		/* Peripheral clock enable */
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
		/**USART1 GPIO Configuration
		PA9   ------> USART1_TX
		PA10   ------> USART1_RX
		*/
		GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
		GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
		GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
		LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
		GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
		LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		/* USART1 interrupt Init */
		NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 4, 0));
		NVIC_EnableIRQ(USART1_IRQn);
		/* USART1 Config */
		USART_InitStruct.BaudRate = bound;
		USART_InitStruct.DataWidth = wordLen;
		USART_InitStruct.StopBits = stopBit;
		USART_InitStruct.Parity = parity;
		USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
		USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
		USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
		LL_USART_Init(USART1, &USART_InitStruct);

		LL_USART_ConfigAsyncMode(USART1);
		LL_USART_Enable(USART1);
		LL_USART_EnableIT_RXNE(USART1); // 接收中断
		LL_USART_EnableIT_PE(USART1);	// 奇偶校验错误中断

		LL_USART_DisableIT_TC(USART1);

		COM1_Dev.inited = 1;
	}
	break;
	case COM2:
	{
		// 创建接收数据缓冲区
		QUEUE_PacketCreate(&m_QueueCom2Rx, m_Com2RxBuf, sizeof(m_Com2RxBuf));
		// 创建发送数据缓冲区
		QUEUE_PacketCreate(&m_QueueCom2Tx, m_Com2TxBuf, sizeof(m_Com2TxBuf));
		/* Peripheral clock enable */
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
		/**USART2 GPIO Configuration
		PA2   ------> USART2_TX
		PA3   ------> USART2_RX
		*/
		GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
		GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
		GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
		LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
		GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
		LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART2 interrupt Init */
		NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
		NVIC_EnableIRQ(USART2_IRQn);
		/* USART2 Config */
		USART_InitStruct.BaudRate = bound;
		USART_InitStruct.DataWidth = wordLen;
		USART_InitStruct.StopBits = stopBit;
		USART_InitStruct.Parity = parity;
		USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
		USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
		USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
		LL_USART_Init(USART2, &USART_InitStruct);

		LL_USART_ConfigAsyncMode(USART2);
		LL_USART_Enable(USART2);
		LL_USART_EnableIT_RXNE(USART2); // 接收中断
		LL_USART_EnableIT_PE(USART2);	// 奇偶校验错误中断
		// LL_USART_EnableIT_TXFE(USART2); //启用TXFIFO Empty中断

		COM2_Dev.inited = 1;
	}
	break;
	case COM3:
	{
		// 创建接收数据缓冲区
		QUEUE_PacketCreate(&m_QueueCom3Rx, m_Com3RxBuf, sizeof(m_Com3RxBuf));
		// 创建发送数据缓冲区
		QUEUE_PacketCreate(&m_QueueCom3Tx, m_Com3TxBuf, sizeof(m_Com3TxBuf));

		LL_USART_InitTypeDef USART_InitStruct = {0};

		LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

		/* Peripheral clock enable */
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
		/**USART3 GPIO Configuration
		PB10   ------> USART3_TX
		PB11   ------> USART3_RX
		*/
		GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
		GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
		GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
		LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
		GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
		LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* USART3 interrupt Init */
		NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
		NVIC_EnableIRQ(USART3_IRQn);

		/* USER CODE BEGIN USART3_Init 1 */

		/* USER CODE END USART3_Init 1 */
		USART_InitStruct.BaudRate = bound;
		USART_InitStruct.DataWidth = wordLen;
		USART_InitStruct.StopBits = stopBit;
		USART_InitStruct.Parity = parity;
		USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
		USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
		USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
		LL_USART_Init(USART3, &USART_InitStruct);
		LL_USART_ConfigAsyncMode(USART3);
		LL_USART_Enable(USART3);

		LL_USART_ConfigAsyncMode(USART3);
		LL_USART_Enable(USART3);
		LL_USART_EnableIT_RXNE(USART3); // 接收中断
		LL_USART_EnableIT_PE(USART3);	// 奇偶校验错误中断
		// LL_USART_EnableIT_TXFE(USART3); //启用TXFIFO Empty中断

		COM3_Dev.inited = 1;
	}
	break;
	default:
		break;
	}
}

/**
 * @brief 串口写操作
 *
 * @param comx 串口号
 * @param writeBuf 存放待写数据缓存区的指针
 * @param uLen 需要写多少个字节
 * @return uint32_t >0-写出去实际字节数，0-未初始化，写失败
 */
uint32_t Uart_Write(COMID_t comid, const uint8_t *writeBuf, uint32_t uLen)
{
	uint32_t uRtn = 0;
	switch (comid)
	{
	case COM1: // 串口1
	{
		if (COM1_Dev.inited == 0)
			uRtn = 0;
		else
			uRtn = uLen;
		if (uRtn > 0)
		{
			uint8_t ch = 0;
			uint32_t push_len = 0;
			for (int i = 0; i < uLen; i++)
			{
				ch = writeBuf[i];
				while (1)
				{
					/* 将新数据填入发送缓冲区 */
					DISABLE_INT();
					push_len = QUEUE_PacketIn(&m_QueueCom1Tx, &ch, 1);
					ENABLE_INT();
					if (push_len > 0)
					{
						break;
					}
					else
					{
						/* 数据已填满缓冲区 */
						/* 如果发送缓冲区已经满了，则等待缓冲区空 */
						LL_USART_EnableIT_TXE(USART1);
					}
				}
			}
			/* 使能发送中断（缓冲区空） */
			LL_USART_EnableIT_TXE(USART1);
		}
	}
	break;
	case COM2: // 串口2
	{
		if (COM2_Dev.inited == 0)
			uRtn = 0;
		else
			uRtn = uLen;
		if (uRtn > 0)
		{
			uint8_t ch = 0;
			uint32_t push_len = 0;
			for (int i = 0; i < uLen; i++)
			{
				ch = writeBuf[i];
				while (1)
				{
					/* 将新数据填入发送缓冲区 */
					DISABLE_INT();
					push_len = QUEUE_PacketIn(&m_QueueCom2Tx, &ch, 1);
					ENABLE_INT();
					if (push_len > 0)
					{
						break;
					}
					else
					{
						/* 数据已填满缓冲区 */
						/* 如果发送缓冲区已经满了，则等待缓冲区空 */
						LL_USART_EnableIT_TXE(USART2);
					}
				}
			}
			/* 使能发送中断（缓冲区空） */
			LL_USART_EnableIT_TXE(USART2);
		}
	}
	break;
	case COM3: // 串口3
	{
		if (COM3_Dev.inited == 0)
			uRtn = 0;
		else
			uRtn = uLen;
		if (uRtn > 0)
		{
			uint8_t ch = 0;
			uint32_t push_len = 0;
			for (int i = 0; i < uLen; i++)
			{
				ch = writeBuf[i];
				while (1)
				{
					/* 将新数据填入发送缓冲区 */
					DISABLE_INT();
					push_len = QUEUE_PacketIn(&m_QueueCom3Tx, &ch, 1);
					ENABLE_INT();
					if (push_len > 0)
					{
						break;
					}
					else
					{
						/* 数据已填满缓冲区 */
						/* 如果发送缓冲区已经满了，则等待缓冲区空 */
						LL_USART_EnableIT_TXE(USART3);
					}
				}
			}
			/* 使能发送中断（缓冲区空） */
			LL_USART_EnableIT_TXE(USART3);
		}
	}
	break;
	default:
		break;
	}
	return uRtn;
}

/**
 * @brief 串口读操作
 *
 * @param comx 串口号
 * @param pBuf 存放读取数据的缓存区的指针
 * @param uiLen 本次操作最多能读取的字节数
 * @return uint32_t >0-实际读取的字节数，0-没有数据或者串口不可用
 */
uint32_t Uart_Read(COMID_t comid, unsigned char *pBuf, uint32_t uiLen)
{

	int uRtn = 0;
	switch (comid)
	{
	case COM1: // 串口1
		uRtn = QUEUE_PacketOut(&m_QueueCom1Rx, pBuf, uiLen);
		break;
	case COM2: // 串口2
		uRtn = QUEUE_PacketOut(&m_QueueCom2Rx, pBuf, uiLen);
		break;
	case COM3: // 串口3
		uRtn = QUEUE_PacketOut(&m_QueueCom3Rx, pBuf, uiLen);
		break;
	default:
		break;
	}
	return uRtn;
}

/**
 * @brief 获取当前串口接收缓存中收到字节数。
 *
 * @param comid
 * @return uint32_t 当前串口接收缓存中收到字节数。
 */
uint32_t Uart_AvailableBytes(COMID_t comid)
{
	int uRtn = 0;
	switch (comid)
	{
	case COM1: // 串口1
		uRtn = QUEUE_PacketLengthGet(&m_QueueCom1Rx);
		break;
	case COM2: // 串口2
		uRtn = QUEUE_PacketLengthGet(&m_QueueCom2Rx);
		break;
	case COM3: // 串口3
		uRtn = QUEUE_PacketLengthGet(&m_QueueCom3Rx);
		break;
	default:
		break;
	}
	return uRtn;
}

/**
 * @brief 清空串口接收缓存。
 *
 * @param comid 串口号。
 * @return uint32_t 成功清空的字节数。
 */
uint32_t Uart_EmptyReadBuffer(COMID_t comid)
{
	int uRtn = 0;
	while (Uart_AvailableBytes(comid) > 0)
	{
		uint8_t buf[32];
		uRtn += Uart_Read(comid, buf, 32);
	}
	return uRtn;
}

/**
 * @brief 注册Uart_Write写入完成回调函数。需要注意的是两次Uart_Write时间间隔太短可能就不能
 * 对应每个Uart_Write调用产生中断。
 *
 * @param comid 串口号。
 * @param callback 回调函数指针。
 * @param args 回调函数参数。
 * @return uint8_t 成功1，失败0.
 */
uint8_t Uart_SetWriteOverCallback(COMID_t comid, UartWriteOverCallback_t callback, void *args)
{
	uint8_t ret = 0;
	switch (comid)
	{
	case COM1: // 串口1
		COM1_Dev.write_over_callback = callback;
		COM1_Dev.write_over_callback_args = args;
		break;
	case COM2: // 串口2
		COM2_Dev.write_over_callback = callback;
		COM2_Dev.write_over_callback_args = args;
		break;
	case COM3: // 串口3
		COM3_Dev.write_over_callback = callback;
		COM3_Dev.write_over_callback_args = args;
		break;
	default:
		ret = 1;
		break;
	}
	return ret;
}

/**
 * @brief
 *
 * @param comid 串口号。
 * @param callback 回调函数指针。
 * @return uint8_t 成功1，失败0.
 */
uint8_t Uart_RegisterReceiveCharCallback(COMID_t comid, UartReceiveCharCallback_t callback)
{
	uint8_t ret = 0;
	switch (comid)
	{
	case COM1: // 串口1
		COM1_Dev.receive_char_callback = callback;
		break;
	case COM2: // 串口2
		COM2_Dev.receive_char_callback = callback;
		break;
	case COM3: // 串口3
		COM3_Dev.receive_char_callback = callback;
		break;
	default:
		ret = 1;
		break;
	}
	return ret;
}

/**
 * @brief 取消注册中断中接收字符数据流的函数，这样就能且只能使用Uart_Read读取数据了。
 *
 * @param comid 串口号。
 * @return uint8_t 成功1，失败0.
 */
uint8_t Uart_UnregisterReceiveCharCallback(COMID_t comid)
{
	uint8_t ret = 0;
	switch (comid)
	{
	case COM1: // 串口1
		COM1_Dev.receive_char_callback = NULL;
		break;
	case COM2: // 串口2
		COM2_Dev.receive_char_callback = NULL;
		break;
	case COM3: // 串口3
		COM3_Dev.receive_char_callback = NULL;
		break;
	default:
		ret = 1;
		break;
	}
	return ret;
}

/**
 * @brief This function handles USART1 global interrupt.
 */
void USART1_IRQHandler(void)
{
	uint8_t buffer[1];
	if (LL_USART_IsActiveFlag_RXNE(USART1) != RESET) // 检测是否接收中断
	{
		buffer[0] = LL_USART_ReceiveData8(USART1); // 读取出来接收到的数据
		// 如果外部单独注册了接收字符数据流的方法，那么就使用外部注册的方法
		if (COM1_Dev.receive_char_callback != NULL)
		{
			COM1_Dev.receive_char_callback(buffer[0]);
		}
		else
		{
			QUEUE_PacketIn(&m_QueueCom1Rx, buffer, 1); // LOOP
		}
	}
	if (LL_USART_IsActiveFlag_ORE(USART1) != RESET)
	{
		buffer[0] = LL_USART_ReceiveData8(USART1);
		LL_USART_ClearFlag_ORE(USART1);
	}
	LL_USART_ClearFlag_FE(USART1); // Clear Framing Error Flag
	LL_USART_ClearFlag_PE(USART1); // 奇偶校验错误清除

	// LL_USART_IsEnabledIT_TC是为了防止接收字符时也进入发送中断
	uint8_t ch = 0;
	uint32_t queque_pop_len = 0;
	/* 处理发送缓冲区空中断 */
	if (LL_USART_IsEnabledIT_TXE(USART1) && LL_USART_IsActiveFlag_TXE(USART1))
	{
		queque_pop_len = QUEUE_PacketOut(&m_QueueCom1Tx, &ch, 1);
		if (queque_pop_len > 0)
		{
			LL_USART_TransmitData8(USART1, ch); // 把数据再从串口发送出去
		}
		else
		{
			/* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
			LL_USART_DisableIT_TXE(USART1);
			/* 使能数据发送完毕中断 */
			LL_USART_EnableIT_TC(USART1);
		}
	}

	/* 数据bit位全部发送完毕的中断 */
	if (LL_USART_IsEnabledIT_TC(USART1) && LL_USART_IsActiveFlag_TC(USART1))
	{
		queque_pop_len = QUEUE_PacketOut(&m_QueueCom1Tx, &ch, 1);
		if (queque_pop_len == 0)
		{
			/* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
			LL_USART_DisableIT_TC(USART1);
			if (COM1_Dev.write_over_callback != NULL)
			{
				COM1_Dev.write_over_callback(COM1_Dev.write_over_callback_args);
			}
		}
		else
		{
			/* 正常情况下，不会进入此分支 */
			/* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
			LL_USART_TransmitData8(USART1, ch); // 把数据再从串口发送出去
		}
	}
}

/**
 * @brief This function handles USART2 global interrupt.
 */
void USART2_IRQHandler(void)
{
	uint8_t buffer[1];
	if (LL_USART_IsActiveFlag_RXNE(USART2) != RESET) // 检测是否接收中断
	{
		buffer[0] = LL_USART_ReceiveData8(USART2); // 读取出来接收到的数据
												   // 如果外部单独注册了接收字符数据流的方法，那么就使用外部注册的方法
		if (COM2_Dev.receive_char_callback != NULL)
		{
			COM2_Dev.receive_char_callback(buffer[0]);
		}
		else
		{
			QUEUE_PacketIn(&m_QueueCom2Rx, buffer, 1); // LOOP
		}
	}
	if (LL_USART_IsActiveFlag_ORE(USART2) != RESET)
	{
		buffer[0] = LL_USART_ReceiveData8(USART2);
		LL_USART_ClearFlag_ORE(USART2);
	}
	LL_USART_ClearFlag_FE(USART2); // Clear Framing Error Flag
	LL_USART_ClearFlag_PE(USART2); // 奇偶校验错误清除
	uint8_t ch = 0;
	uint32_t queque_pop_len = 0;
	/* 处理发送缓冲区空中断 */
	if (LL_USART_IsEnabledIT_TXE(USART2) && LL_USART_IsActiveFlag_TXE(USART2))
	{
		queque_pop_len = QUEUE_PacketOut(&m_QueueCom2Tx, &ch, 1);
		if (queque_pop_len > 0)
		{
			LL_USART_TransmitData8(USART2, ch); // 把数据再从串口发送出去
		}
		else
		{
			/* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
			LL_USART_DisableIT_TXE(USART2);
			/* 使能数据发送完毕中断 */
			LL_USART_EnableIT_TC(USART2);
		}
	}

	/* 数据bit位全部发送完毕的中断 */
	if (LL_USART_IsEnabledIT_TC(USART2) && LL_USART_IsActiveFlag_TC(USART2))
	{
		queque_pop_len = QUEUE_PacketOut(&m_QueueCom2Tx, &ch, 1);
		if (queque_pop_len == 0)
		{
			/* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
			LL_USART_DisableIT_TC(USART2);
			if (COM2_Dev.write_over_callback != NULL)
			{
				COM2_Dev.write_over_callback(COM1_Dev.write_over_callback_args);
			}
		}
		else
		{
			/* 正常情况下，不会进入此分支 */
			/* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
			LL_USART_TransmitData8(USART2, ch); // 把数据再从串口发送出去
		}
	}
}

/**
 * @brief This function handles USART3 global interrupt.
 */
void USART3_IRQHandler(void)
{
	uint8_t buffer[1];
	if (LL_USART_IsActiveFlag_RXNE(USART3) != RESET) // 检测是否接收中断
	{
		buffer[0] = LL_USART_ReceiveData8(USART3); // 读取出来接收到的数据
												   // 如果外部单独注册了接收字符数据流的方法，那么就使用外部注册的方法
		if (COM3_Dev.receive_char_callback != NULL)
		{
			COM3_Dev.receive_char_callback(buffer[0]);
		}
		else
		{
			QUEUE_PacketIn(&m_QueueCom3Rx, buffer, 1); // LOOP
		}
	}
	if (LL_USART_IsActiveFlag_ORE(USART3) != RESET)
	{
		buffer[0] = LL_USART_ReceiveData8(USART3);
		LL_USART_ClearFlag_ORE(USART3);
	}
	LL_USART_ClearFlag_FE(USART3); // Clear Framing Error Flag
	LL_USART_ClearFlag_PE(USART3); // 奇偶校验错误清除
	uint8_t ch = 0;
	uint32_t queque_pop_len = 0;
	/* 处理发送缓冲区空中断 */
	if (LL_USART_IsEnabledIT_TXE(USART3) && LL_USART_IsActiveFlag_TXE(USART3))
	{
		queque_pop_len = QUEUE_PacketOut(&m_QueueCom3Tx, &ch, 1);
		if (queque_pop_len > 0)
		{
			LL_USART_TransmitData8(USART3, ch); // 把数据再从串口发送出去
		}
		else
		{
			/* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
			LL_USART_DisableIT_TXE(USART3);
			/* 使能数据发送完毕中断 */
			LL_USART_EnableIT_TC(USART3);
		}
	}

	/* 数据bit位全部发送完毕的中断 */
	if (LL_USART_IsEnabledIT_TC(USART3) && LL_USART_IsActiveFlag_TC(USART3))
	{
		queque_pop_len = QUEUE_PacketOut(&m_QueueCom3Tx, &ch, 1);
		if (queque_pop_len == 0)
		{
			/* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
			LL_USART_DisableIT_TC(USART3);
			if (COM3_Dev.write_over_callback != NULL)
			{
				COM3_Dev.write_over_callback(COM3_Dev.write_over_callback_args);
			}
		}
		else
		{
			/* 正常情况下，不会进入此分支 */
			/* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
			LL_USART_TransmitData8(USART3, ch); // 把数据再从串口发送出去
		}
	}
}