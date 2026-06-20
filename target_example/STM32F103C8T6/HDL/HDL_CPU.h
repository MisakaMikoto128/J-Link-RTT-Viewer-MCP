/**
* @file HDL_CPU.h
* @author Liu Yuanlin (liuyuanlins@outlook.com)
* @brief
* @version 0.1
* @date 2023-01-11
* @last modified 2023-01-11
*
* @copyright Copyright (c) 2023 Liu Yuanlin Personal.
*
*/
#ifndef HDL_CPU_H
#define HDL_CPU_H

/* 开关全局中断的宏 */
#define ENABLE_INT() __set_PRIMASK(0)  /* 使能全局中断 */
#define DISABLE_INT() __set_PRIMASK(1) /* 禁止全局中断 */

#endif //!HDL_CPU_H