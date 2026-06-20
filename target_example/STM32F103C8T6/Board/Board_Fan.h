/**
* @file Board_Fan.h
* @author Liu Yuanlin (liuyuanlins@outlook.com)
* @brief
* @version 0.1
* @date 2025-12-26
* @last modified 2025-12-26
*
* @copyright Copyright (c) 2025 Liu Yuanlin Personal.
*
*/
#ifndef BOARD_FAN_H
#define BOARD_FAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void Board_Fan_Init();

/**
 * @brief 
 * 
 * @param rpm 转速，单位RPM 
 */
void Board_Set_Fan_Speed(uint32_t rpm);
#ifdef __cplusplus
}
#endif
#endif //!BOARD_FAN_H
