/**
 * @file HDL_Flash.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-10-12
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */
#ifndef HDL_FLASH_H
#define HDL_FLASH_H
#include "main.h"
#include "bit_operation.h"
/*
对于有两个BANK的情况，BANK1开始的page作为第0个扇区，BANK2的起始扇区号就是
BANK1结束扇区号+1.

HDL的sector定义为最小的擦除单位，对于STM32F103来说，最小擦除单位是page，
*/
/**
 * @brief  Flash info.
 *
 */
#define HDL_FLASH_SECTOR_SIZE FLASH_PAGE_SIZE                        // 单位：字节
#define HDL_FLASH_SIZE (LL_GetFlashSize() * 1024UL)                  // 单位：字节
#define HDL_FLASH_SECTOR_NB (HDL_FLASH_SIZE / HDL_FLASH_SECTOR_SIZE) // FLASH物理扇区数量
#define HDL_FLASH_START_ADDRESS FLASH_BASE                               // FLASH物理起始地址
#define HDL_FLASH_END_ADDRESS (HDL_FLASH_START_ADDRESS + HDL_FLASH_SIZE)    // FLASH物理结束地址
//[HDL_FLASH_START_ADDRESS, HDL_FLASH_END_ADDRESS)

#define HDL_FLASH_READ_8BIT (8UL)
#define HDL_FLASH_READ_16BIT (16UL)
#define HDL_FLASH_READ_32BIT (32UL)
#define HDL_FLASH_READ_64BIT (64UL)
#define HDL_FLASH_READ_128BIT (128UL)

/*In stm32 mcu, the flash read bit width alaways 32bit*/
#define HDL_FLASH_READ_BIT_WIDTHT HDL_FLASH_READ_32BIT
/*
Flash编程并行位数。对STM32F103来说，一次可以写入16bit。
 */
#define HDL_FLASH_PROGRAMM_BIT_WIDTHT 16UL

/**
 * @brief some define for flash operation
 *
 */
#define HDL_FLASH_READ_SIZE_ALIGN(size) (ALIGN_UP(size, HDL_FLASH_READ_BIT_WIDTHT / 8))
#define HDL_FLASH_PROGRAMM_BYTE_WIDTHT (HDL_FLASH_PROGRAMM_BIT_WIDTHT / 8)

/**
 * @brief 得到扇区对应的首地址。
 *
 */
#define HDL_FLASH_ADDR_OF_SECTOR(sector) (HDL_FLASH_START_ADDRESS + (sector)*HDL_FLASH_SECTOR_SIZE)
#define HDL_SECTOR_OF_ADDRESS(address) (((address)-HDL_FLASH_START_ADDRESS) / HDL_FLASH_SECTOR_SIZE)

int HDL_Flash_init();
int HDL_Flash_write(uint32_t address, uint8_t *data, uint32_t size);
int HDL_Flash_read(uint32_t address, uint8_t *buf, uint32_t size);

/* Private */
int HDL_Flash_erase_sector(uint32_t sector, uint32_t count);
#endif //! HDL_FLASH_H
