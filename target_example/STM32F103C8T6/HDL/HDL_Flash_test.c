/**
 * @file HDL_Flash_test.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-10-12
 *
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 *
 */
#include "HDL_Flash_test.h"
#include "log.h"
#include <string.h>

static uint8_t test_buf[HDL_FLASH_SECTOR_SIZE] = {0};
static uint8_t test_buf2[100] = {0};
static uint8_t test_buf3[100] = {0};
void Flash_inner_fun_test()
{
    ulog_init_user();
    HDL_Flash_init();
    ULOG_DEBUG("HDL_FLASH_SECTOR_NB : %u", HDL_FLASH_SECTOR_NB);
    ULOG_DEBUG("HDL_FLASH_SECTOR_SIZE : %u byte", HDL_FLASH_SECTOR_SIZE);
    ULOG_DEBUG("HDL_FLASH_SIZE : %u Kbyte", HDL_FLASH_SIZE/1024UL);
    ULOG_DEBUG("HDL_FLASH_START_ADDRESS : %#X", HDL_FLASH_START_ADDRESS);
    ULOG_DEBUG("HDL_FLASH_END_ADDRESS : %#X", HDL_FLASH_END_ADDRESS);

    uint32_t sector = HDL_FLASH_SECTOR_NB - 3;
    __IO uint32_t addr = HDL_Flash_get_address_by_sector(sector);

    ULOG_DEBUG("test sector : %u , address : %#X", sector, addr);

    HDL_Flash_erase_one_sector(sector);
    //保证后面一个扇区被擦除，用来看写一个扇区会不会导致另一个扇区数据被擦除。
    HDL_Flash_erase_one_sector(sector + 1);
    for (size_t i = 0; i < HDL_FLASH_SECTOR_SIZE; i++)
    {
        test_buf[i] = i;
    }
    HDL_Flash_write_one_sector_nocheck(sector, test_buf);
    memset(test_buf, 0, sizeof(test_buf)/sizeof(test_buf[0]));
    HDL_Flash_read(addr, test_buf, HDL_FLASH_SECTOR_SIZE);

    uint32_t error_cnt = 0;
    for (size_t i = 0; i < HDL_FLASH_SECTOR_SIZE; i++)
    {
        if (test_buf[i] != (uint8_t)i)
        {
            error_cnt++;
        }
    }
    ULOG_DEBUG("test sector : %u , error cnt : %u; Test : %s", sector, error_cnt, error_cnt == 0 ? "pass" : "fail");
    sector++;
    error_cnt = 0;
    addr = HDL_Flash_get_address_by_sector(sector);
    HDL_Flash_read(addr, test_buf, HDL_FLASH_SECTOR_SIZE);
    for (size_t i = 0; i < HDL_FLASH_SECTOR_SIZE; i++)
    {
        if (test_buf[i] != 0xFF)
        {
            error_cnt++;
        }
    }
    ULOG_DEBUG("test sector : %u , error cnt : %u; Test : %s", sector, error_cnt, error_cnt == 0 ? "pass" : "fail");
    while(1)
    {
        
    }
}

void Flash_test()
{
    ulog_init_user();
    HDL_Flash_init();
    /*
    测试写1/4个扇区，写之前先读取扇区剩余部分的数据有没有被修改。
    */
    uint32_t sector = HDL_FLASH_SECTOR_NB - 3;
    uint32_t start_addr = HDL_Flash_get_address_by_sector(sector);

    for (size_t i = 0; i < HDL_FLASH_SECTOR_SIZE; i++)
    {
        test_buf[i] = (uint8_t)(i * i);
    }

    HDL_Flash_read(start_addr, test_buf2, 100);
    HDL_Flash_read(start_addr + 100 + HDL_FLASH_SECTOR_SIZE / 4, test_buf3, 100);
    HDL_Flash_write(start_addr + 100, test_buf, HDL_FLASH_SECTOR_SIZE / 4);
    //清零缓存区域
    memset(test_buf, 0, HDL_FLASH_SECTOR_SIZE);
    HDL_Flash_read(start_addr, test_buf, HDL_FLASH_SECTOR_SIZE);
    uint32_t error_cnt = 0;
    for (size_t i = 0; i < 100; i++)
    {
        if (test_buf[i] != test_buf2[i])
        {
            error_cnt++;
        }
    }

    ULOG_DEBUG("the first 100 bytes,test sector : %u , error cnt : %u; Test : %s", sector, error_cnt, error_cnt == 0 ? "pass" : "fail");
    error_cnt = 0;

    for (size_t i = 0; i < 100; i++)
    {
        if (test_buf[i + 100 + HDL_FLASH_SECTOR_SIZE / 4] != test_buf3[i])
        {
            error_cnt++;
        }
    }

    ULOG_DEBUG("the last 100 bytes,test sector : %u , error cnt : %u; Test : %s", sector, error_cnt, error_cnt == 0 ? "pass" : "fail");
    error_cnt = 0;
    size_t tmp = 0;
    for (size_t i = 100; i < HDL_FLASH_SECTOR_SIZE / 4 + 100; i++)
    {
				tmp = i - 100;
        if (test_buf[i] != (uint8_t)(tmp * tmp))
        {
            error_cnt++;
        }
    }

    ULOG_DEBUG("the 1/4 sector bytes,test sector : %u , error cnt : %u; Test : %s", sector, error_cnt, error_cnt == 0 ? "pass" : "fail");
    error_cnt = 0;
		while(1)
		{
			
		}
}