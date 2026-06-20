/**
 * @file Test.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-03-19
 * @last modified 2023-03-19
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#include "Test.h"

#include "HDL_Uart.h"
#include "HDL_Uart_test.h"
#include "HDL_CPU_Time_test.h"
#include "log.h"
#include "HDL_RTC_test.h"
#include "BFL_LED.h"
#include "APP_Main.h"
#include "HDL_PWM_test.h"
#include "HDL_ADC_test.h"
#include "BFL_Buzz.h"
#include "mtime.h"
#include "log.h"


void Buzz_Test()
{
  HDL_CPU_Time_Init();
  ulog_init_user();
  BFL_Buzz_setTrigLevel(HDL_GPIO_HIGH);
  BFL_Buzz_init();

  while (1)
  {
    // 1s 执行一次
    // if (period_query(1, 1000))
    // {
    //   BFL_Buzz_toggle();
    //   //ULOG_INFO("[Buzz Test] %s", BFL_Buzz_isOff() == true ? "OFF" : "ON");
    // }
    BFL_Buzz_on();
  }
}

void Test(void)
{
  // Buzz_Test();
  // CHIP_HX711_test();
  // CHIP_DHT11_test();
	// CHIP_BH1750_Test();
  // CHIP_SHT30_poll_test();
  // CHIP_SHT30_async_test();
  // HDL_ADC_test();
  // CHIP_RC522_test();
  // CHIP_DS18B20_test();
  // CHIP_OLED_fps_test();
	// CHIP_VL6180X_test();
  // CHIP_IoE_SR05_Test();
}