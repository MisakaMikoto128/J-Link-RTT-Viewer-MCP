/**
* @file Board.h
* @author Liu Yuanlin (liuyuanlins@outlook.com)
* @brief
* @version 0.1
* @date 2025-12-17
* @last modified 2025-12-17
*
* @copyright Copyright (c) 2025 Liu Yuanlin Personal.
*
*/
#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "BFL_RS485422.h"
#include "HDL_CAN.h"

extern BFL_RS485422Dev_t *RS485422_1;

void Board_Init();
void Board_Config_CAN_120_EN(bool enable);
void Board_Config_CAN_EN(bool enable);
void Board_Config_485_T120_EN(bool enable);
void Board_Config_485_R120_EN(bool enable);
void Board_Config_485_Semiduplex_EN(bool enable);

#ifdef __cplusplus
}
#endif
#endif //!BOARD_H
