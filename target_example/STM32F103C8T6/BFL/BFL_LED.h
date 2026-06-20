/**
 * @file BFL_LED.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-13
 * 
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 * 
 */
#ifndef BFL_LED_H
#define BFL_LED_H

typedef enum
{
    LED1 = 1,
    LED2 = 2,
} BFL_LED_ID;

void BFL_LED_init();
void BFL_LED_on(int led_id);
void BFL_LED_off(int led_id);
void BFL_LED_toggle(int led_id);
#endif //!BFL_LED_H