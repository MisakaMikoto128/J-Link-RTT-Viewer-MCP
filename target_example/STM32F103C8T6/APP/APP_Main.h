/**
 * @file APP_Main.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-11
 * @last modified 2023-01-11
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#ifndef APP_MAIN_H
#define APP_MAIN_H
#include <stdint.h>
#include <stdbool.h>


//@user deination 1
typedef struct
{
    // 基本系统信息
    uint32_t devid;
    uint32_t product_key;
} Sysinfo_t;

void APP_Main();
uint32_t APP_Main_GetDevId();
uint32_t APP_Main_GetProductKey();
#endif //! APP_MAIN_H