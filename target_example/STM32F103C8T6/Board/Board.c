/**
 * @file Board.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2025-12-17
 * @last modified 2025-12-17
 *
 * @copyright Copyright (c) 2025 Liu Yuanlin Personal.
 *
 */
#include "Board.h"
#include "Board_Fan.h"
#include "HDL_GPIO.h"
#include "HDL_PWM.h"
#include "asyn_sys.h"

#include "main.h"

BFL_RS485422Dev_t RS4854221_Obj = {
    .deGPIOx    = GPIOB,
    .deGPIO_Pin = LL_GPIO_PIN_13,
    .reGPIOx    = GPIOB,
    .reGPIO_Pin = LL_GPIO_PIN_12,
    .comID      = COM3,
    .baud       = 115200,
    .parity     = 'N',
    .dataBit    = 8,
    .stopBit    = 1,
    .mode       = BFL_RS485422_MODE_RS485,
};

BFL_RS485422Dev_t *RS485422_1 = &RS4854221_Obj;

void Board_Init()
{
    LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

    /**/
    LL_GPIO_ResetOutputPin(LED0_GPIO_Port, LED0_Pin);

    /**/
    LL_GPIO_ResetOutputPin(GPIOA, IO_CAN_120_EN_Pin | IO_485_HALF_EN_Pin);

    /**/
    LL_GPIO_ResetOutputPin(GPIOB, IO_485_R120_EN_Pin | IO_485_T120_EN_Pin | IO_CAN_EN_Pin);

    /**/
    GPIO_InitStruct.Pin        = LED0_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin        = IO_CAN_120_EN_Pin | IO_485_HALF_EN_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin        = IO_485_R120_EN_Pin | IO_485_T120_EN_Pin | IO_CAN_EN_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // IO_FAN_SENSE_Pin 初始化，通过中断计数来统计频率。
    /**/
    LL_GPIO_AF_SetEXTISource(LL_GPIO_AF_EXTI_PORTA, LL_GPIO_AF_EXTI_LINE9);

    /**/
    EXTI_InitStruct.Line_0_31   = LL_EXTI_LINE_9;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode        = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger     = LL_EXTI_TRIGGER_RISING;
    LL_EXTI_Init(&EXTI_InitStruct);

    /**/
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_FLOATING);

    /* EXTI interrupt init*/
    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    BFL_RS485422_Init(RS485422_1);
    if (RS485422_1->mode == BFL_RS485422_MODE_RS485) {
        Board_Config_485_Semiduplex_EN(false);
    } else {
        Board_Config_485_Semiduplex_EN(true);
    }

    Board_Config_CAN_120_EN(true);
    Board_Config_485_T120_EN(true);
    Board_Config_485_R120_EN(true);
    Board_Config_CAN_EN(true);
    HDL_CAN_Init(CAN_DEV_1, CAN_BAUD_125K);

    HDL_PWM_Init(PWM_1);

    HDL_PWM_SetDuty(PWM_1, 1.0f);

    Board_Fan_Init();
}

void Board_Config_CAN_120_EN(bool enable)
{
    if (enable) {
        LL_GPIO_SetOutputPin(IO_CAN_120_EN_GPIO_Port, IO_CAN_120_EN_Pin);
    } else {
        LL_GPIO_ResetOutputPin(IO_CAN_120_EN_GPIO_Port, IO_CAN_120_EN_Pin);
    }
}

void Board_Config_CAN_EN(bool enable)
{
    // LOW Normal, HIGH Silent
    if (enable) {
        LL_GPIO_ResetOutputPin(IO_CAN_EN_GPIO_Port, IO_CAN_EN_Pin);
    } else {
        LL_GPIO_SetOutputPin(IO_CAN_EN_GPIO_Port, IO_CAN_EN_Pin);
    }
}

void Board_Config_485_T120_EN(bool enable)
{
    if (enable) {
        LL_GPIO_SetOutputPin(IO_485_T120_EN_GPIO_Port, IO_485_T120_EN_Pin);
    } else {
        LL_GPIO_ResetOutputPin(IO_485_T120_EN_GPIO_Port, IO_485_T120_EN_Pin);
    }
}

void Board_Config_485_R120_EN(bool enable)
{
    if (enable) {
        LL_GPIO_SetOutputPin(IO_485_R120_EN_GPIO_Port, IO_485_R120_EN_Pin);
    } else {
        LL_GPIO_ResetOutputPin(IO_485_R120_EN_GPIO_Port, IO_485_R120_EN_Pin);
    }
}

void Board_Config_485_Semiduplex_EN(bool enable)
{
    if (enable) {
        LL_GPIO_SetOutputPin(IO_485_HALF_EN_GPIO_Port, IO_485_HALF_EN_Pin);
    } else {
        LL_GPIO_ResetOutputPin(IO_485_HALF_EN_GPIO_Port, IO_485_HALF_EN_Pin);
    }
}
