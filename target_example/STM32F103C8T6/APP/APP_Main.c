/**
 * @file APP_Main.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-11
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */
#include <stdio.h>
#include <stdint.h>
#include "log.h"
#include "APP_Main.h"
#include "BFL_LED.h"
#include "asyn_sys.h"
#include "mtime.h"
#include "HDL_RTC.h"
#include "HDL_PWM.h"
#include "SEGGER_RTT.h"
#include "HDL_CPU_Time.h"
#include "test_lib.h"
#include <stdio.h>
#include <string.h>
#include "HDL_ADC.h"
#include "HDL_Uart.h"
#include "cJSON.h"
#include <ctype.h>
#include "alarm_clock.h"
#include <math.h>
#include <stdlib.h>
#include "HDL_CPU.h"
#include "HDL_Flash.h"
#include "str.h"
#include "Key.h"
#include "Board.h"

void SysPoll();
void IoTReloadSavedData();
void IoTSaveData();
void AppMainPoll();
//@user deination 2

#define SAMPLE_POINT_FLASH_BASE (HDL_FLASH_END_ADDRESS - 2 * HDL_FLASH_SECTOR_SIZE)
typedef struct
{
    uint32_t tag;
    Sysinfo_t g_sysinfo;
} SamplePoint_t;

// 全局系统信息变量
Sysinfo_t g_sysinfo = {
    // 基本系统信息
    .devid       = 0x00000001UL,
    .product_key = 0x00000001UL,
};

SamplePoint_t g_samplePoint = {0};

//@user deination 4
void InTimePoll()
{
    KeyScan();
    static int i = 0;
    if (++i >= 1000) {
        i = 0;
    }
}

/* APP_Main 初始化顺序从这开始 */
void SysinfoInit()
{
    IoTReloadSavedData();
}

//@user deination 5
void SysPoll()
{
    static DelayREC_t delay_rec = {0};
    static PeriodREC_t oneMS    = {0};
    if (delay_one_times(&delay_rec, 10)) {
    }

    uint8_t buf[32] = {0};
    if (BFL_RS485422_AvailableBytes(RS485422_1) > 0) {
        // HDL_CPU_Time_DelayMs(10);
        size_t ret = BFL_RS485422_Read(RS485422_1, buf, sizeof(buf));
        BFL_RS485422_Write(RS485422_1, buf, ret);
    }

    CAN_Frame_t frame;
    if (HDL_CAN_Read(CAN_DEV_1, &frame)) {
      HDL_CAN_Write(CAN_DEV_1, &frame);
    }

    if (period_query_user(&oneMS, 1000)) {
        frame.id      = 0x123;
        frame.id_type = HDL_CAN_ID_EXT;
        frame.data[0] = 0x55;
        frame.data[1] = 0xCC;
        frame.data[2] = 0xAA;
        frame.dlc     = 3;
        //HDL_CAN_Write(CAN_DEV_1, &frame);

    }
}

void SysBaseFunctionInit()
{
    HDL_CPU_Time_Init();
    HDL_RTC_Init();
    HDL_Flash_init();
    BFL_LED_init();
    ulog_init_user();
    //HDL_CPU_Time_DelayMs(1000);
}

void IoTControlerInit()
{
    const KeyPin_t keyinpins[] = {
        {GPIOA, GPIO_PIN_7, KEY_IN_LOW},
        {GPIOB, GPIO_PIN_0, KEY_IN_LOW},
        {GPIOB, GPIO_PIN_1, KEY_IN_LOW},
        {GPIOB, GPIO_PIN_2, KEY_IN_LOW},
    };

    KeyInit(sizeof(keyinpins) / sizeof(keyinpins[0]), 0, keyinpins, NULL);
    enable_key_up_envent(false);
    HDL_CPU_Time_SetCPUTickCallback(InTimePoll);
}

//@user deination 8
void IoTGetPhysicalPropertys()
{
    static PeriodREC_t s_tUploadTime = 0;

    // 每500ms尝试上传一次数据
    if (period_query_user(&s_tUploadTime, 500)) {
        BFL_LED_toggle(LED1);
    }
}

void keyHandler(KeyState_t key)
{
    ULOG_INFO("[Key]:ID : %u event %s", KeyGetKeyID(key), KeyGetEventName(key));
    switch (key) {
        case KEY1_Down:
            ULOG_INFO("Key1 Down");
            break;
        case KEY2_Down:
            ULOG_INFO("Key2 Down");
            break;
        case KEY3_Down:
            ULOG_INFO("Key3 Down");
            break;
        case KEY4_Down:
            ULOG_INFO("Key4 Down");
            break;
        case KEY1_LongPress:
            ULOG_INFO("Key1 LongPress");
            break;
        case KEY2_LongPress:
            ULOG_INFO("Key2 LongPress");
            break;
        case KEY3_LongPress:
            ULOG_INFO("Key3 LongPress");
            break;
        case KEY4_LongPress:
            ULOG_INFO("Key4 LongPress");
            break;
        case KEY1_DoubleClick:
            ULOG_INFO("Key1 DoublePress");
            break;
        case KEY2_DoubleClick:
            ULOG_INFO("Key2 DoublePress");
            break;
        case KEY3_DoubleClick:
            ULOG_INFO("Key3 DoublePress");
            break;
        case KEY4_DoubleClick:
            ULOG_INFO("Key4 DoublePress");
            break;
        default:
            break;
    }
}

//@user deination 11
void IoTCloudMessageHandler(const cJSON *monitor_json)
{
    const cJSON *jsonObj = NULL;

    IoTSaveData();
}

/*  固定不变的逻辑部分 */
void AppMainPoll()
{
    if (!isKeyFIFOEmpty()) {
        keyHandler(Key_FIFO_Get());
    }
    IoTGetPhysicalPropertys();
}

void APP_Main()
{
    SysinfoInit();
    
    SysBaseFunctionInit();
    ULOG_INFO("STM32F103C8T6");
    IoTControlerInit();
    Board_Init();

    asyn_sys_register(SysPoll);
    asyn_sys_register(AppMainPoll);
    while (1) {
        asyn_sys_poll();
    }
}

uint32_t APP_Main_GetDevId()
{
    return g_sysinfo.devid;
}

uint32_t APP_Main_GetProductKey()
{
    return g_sysinfo.product_key;
}

void IoTReloadSavedData()
{
    HDL_Flash_read(SAMPLE_POINT_FLASH_BASE, (uint8_t *)&g_samplePoint, sizeof(g_samplePoint));
    if (g_samplePoint.tag == 0x55AA55AA) {
        g_sysinfo = g_samplePoint.g_sysinfo;
    } else {
        g_samplePoint.tag       = 0x55AA55AA;
        g_samplePoint.g_sysinfo = g_sysinfo;
        IoTSaveData();
    }
}

void IoTSaveData()
{
    g_samplePoint.g_sysinfo = g_sysinfo;
    HDL_Flash_write(SAMPLE_POINT_FLASH_BASE, (uint8_t *)&g_samplePoint, sizeof(g_samplePoint));
}