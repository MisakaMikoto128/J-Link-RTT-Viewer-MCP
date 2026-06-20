/**
 * @file HDL_CAN.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief CAN hardware abstraction layer for STM32F103
 * @version 0.2
 * @date 2024-02-29
 * @last modified 2025-12-17
 *
 * @copyright Copyright (c) 2024 Liu Yuanlin Personal.
 *
 */
#ifndef HDL_CAN_H
#define HDL_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* CAN设备ID枚举 */
typedef enum {
    CAN_DEV_1 = 0,
    CAN_DEV_NUM
} CAN_DevID_t;

/* CAN ID类型 */
typedef enum {
    HDL_CAN_ID_STD = 0,  /* 标准帧 11-bit ID (0x000-0x7FF) */
    HDL_CAN_ID_EXT = 1   /* 扩展帧 29-bit ID (0x00000000-0x1FFFFFFF) */
} CAN_IDType_t;

/* CAN帧结构体 */
typedef struct {
    uint32_t id;           /* CAN ID */
    CAN_IDType_t id_type;  /* ID类型：标准帧或扩展帧 */
    uint8_t dlc;           /* 数据长度 0-8 */
    uint8_t data[8];       /* 数据缓冲区 */
} CAN_Frame_t;

/* CAN波特率枚举 */
typedef enum {
    CAN_BAUD_1M = 0,
    CAN_BAUD_500K,
    CAN_BAUD_250K,
    CAN_BAUD_125K,
    CAN_BAUD_100K,
    CAN_BAUD_50K,
} CAN_BaudRate_t;

/**
 * @brief 初始化CAN设备
 * @param dev_id CAN设备ID
 * @param baudrate CAN波特率
 */
void HDL_CAN_Init(CAN_DevID_t dev_id, CAN_BaudRate_t baudrate);

/**
 * @brief 发送CAN帧
 * @param dev_id CAN设备ID
 * @param frame CAN帧指针
 * @return 1-成功, 0-失败
 */
uint8_t HDL_CAN_Write(CAN_DevID_t dev_id, const CAN_Frame_t *frame);

/**
 * @brief 接收CAN帧
 * @param dev_id CAN设备ID
 * @param frame CAN帧指针
 * @return 1-成功, 0-无数据
 */
uint8_t HDL_CAN_Read(CAN_DevID_t dev_id, CAN_Frame_t *frame);

/**
 * @brief 获取接收队列中可用消息数量
 * @param dev_id CAN设备ID
 * @return 可用消息数量
 */
uint16_t HDL_CAN_AvailableMsgNum(CAN_DevID_t dev_id);

#ifdef __cplusplus
}
#endif
#endif /* HDL_CAN_H */
