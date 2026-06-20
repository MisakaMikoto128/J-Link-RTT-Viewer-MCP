/**
 * @file HDL_SPI.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-19
 * @last modified 2023-01-19
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#ifndef HDL_SPI_H
#define HDL_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>
#include <stdbool.h>
typedef enum {
    SPI_1 = 0,
    SPI_NUM,
} SPI_ID_t;

#define HDL_SPI_MAX_BIT_RATE 18000000UL // MBits/s

void HDL_SPI_Init(SPI_ID_t spiID, uint8_t dataSize, uint32_t CPOL, uint32_t CPHA);
bool HDL_SPI_WriteRead(SPI_ID_t spiID, uint8_t *pTxData, uint8_t *pRxData, uint16_t size);
bool HDL_SPI_Write(SPI_ID_t spiID, uint8_t *pTxData, uint16_t size);
bool HDL_SPI_Read(SPI_ID_t spiID, uint8_t *pRxData, uint16_t size);
void HDL_SPI_DeInit(SPI_ID_t spiID);

#ifdef __cplusplus
}
#endif
#endif //! HDL_SPI_H
