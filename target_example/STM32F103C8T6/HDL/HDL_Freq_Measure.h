/**
* @file HDL_Freq_Measure.h
* @author Liu Yuanlin (liuyuanlins@outlook.com)
* @brief
* @version 0.1
* @date 2025-12-26
* @last modified 2025-12-26
*
* @copyright Copyright (c) 2025 Liu Yuanlin Personal.
*
*/
#ifndef HDL_FREQ_MEASURE_H
#define HDL_FREQ_MEASURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
typedef enum {
    FREQ_MEASURE_1 = 0,
    FREQ_MEASURE_NUM,
} FREQ_MEASURE_ID_t;

typedef uint16_t PluseCnt_t;
void HDL_Freq_Measure_Init(FREQ_MEASURE_ID_t freqMeasureID);

PluseCnt_t HDL_Freq_Measure_GetPlusCount(FREQ_MEASURE_ID_t freqMeasureID);

void HDL_Freq_Measure_ResetPlusCount(FREQ_MEASURE_ID_t freqMeasureID);
#ifdef __cplusplus
}
#endif
#endif //!HDL_FREQ_MEASURE_H
