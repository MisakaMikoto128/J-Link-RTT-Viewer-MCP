/**
 * @file HDL_Uart_test.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-10-05
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */
#include "HDL_Uart_test.h"
#include "HDL_CPU_Time.h"
/**
 * @brief 串口测试方法，该函数会一直阻塞执行
 *
 */
void HDL_Uart_test_loop()
{
	uint8_t buf[10] = {0};
	uint32_t data_len = 0;

	Uart_Init(COM1, 115200, LL_USART_DATAWIDTH_8B, LL_USART_STOPBITS_1, LL_USART_PARITY_NONE);
	Uart_Init(COM2, 115200, LL_USART_DATAWIDTH_8B, LL_USART_STOPBITS_1, LL_USART_PARITY_NONE);
	Uart_Init(COM3, 115200, LL_USART_DATAWIDTH_8B, LL_USART_STOPBITS_1, LL_USART_PARITY_NONE);

	while (1)
	{
		HAL_Delay(500);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		for (COMID_t com = COM1; com < COM_NUM; com++)
		{
			data_len = Uart_Read(com, buf, sizeof(buf));
			if (data_len > 0)
			{
				Uart_Write(com, buf, data_len);
			}
		}
	}
}

int stdout_putchar(int ch)
{
	while (!LL_USART_IsActiveFlag_TC(USART1))
	{
	}
	LL_USART_TransmitData8(USART1, ch);
	return ch;
}
