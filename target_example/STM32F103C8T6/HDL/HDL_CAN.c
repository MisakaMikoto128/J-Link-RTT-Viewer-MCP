/**
 * @file HDL_CAN.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief CAN hardware abstraction layer implementation for STM32F103
 * @version 0.2
 * @date 2024-02-29
 * @last modified 2025-12-17
 *
 * @copyright Copyright (c) 2024 Liu Yuanlin Personal.
 *
 */
#include "HDL_CAN.h"
#include "HDL_CPU.h"
#include "sc_cqueue.h"
#include "can.h"
#include "main.h"
sc_cqueue_def(CAN_Frame_t, can_frame_t);
static CAN_HandleTypeDef mhcan;

/* CAN设备结构体定义 */
typedef struct {
    struct sc_cqueue_can_frame_t tx_queue;
    struct sc_cqueue_can_frame_t rx_queue;
    CAN_Frame_t *tx_buf;
    CAN_Frame_t *rx_buf;
    CAN_HandleTypeDef *hcan;
    bool is_init;
} CAN_Device_t;

/* 发送和接收队列缓冲区 */
#define CAN_TX_QUEUE_SIZE 20
#define CAN_RX_QUEUE_SIZE 20

static CAN_Frame_t g_can_tx_buf[CAN_TX_QUEUE_SIZE];
static CAN_Frame_t g_can_rx_buf[CAN_RX_QUEUE_SIZE];

/* CAN设备列表 */
static CAN_Device_t g_can_dev_list[CAN_DEV_NUM] = {0};

/* 波特率配置表 (APB1=36MHz)
 * CAN波特率 = APB1 / (Prescaler * (1 + BS1 + BS2))
 * 采样点位置 = (1 + BS1) / (1 + BS1 + BS2)
 */
typedef struct {
    uint32_t prescaler;
    uint32_t bs1;
    uint32_t bs2;
    uint32_t sjw;
} CAN_BaudConfig_t;

static const CAN_BaudConfig_t g_baud_config[] = {
    [CAN_BAUD_1M]   = {4, CAN_BS1_6TQ, CAN_BS2_2TQ, CAN_SJW_1TQ},  /* 1Mbps */
    [CAN_BAUD_500K] = {8, CAN_BS1_6TQ, CAN_BS2_2TQ, CAN_SJW_1TQ},  /* 500Kbps */
    [CAN_BAUD_250K] = {16, CAN_BS1_6TQ, CAN_BS2_2TQ, CAN_SJW_1TQ}, /* 250Kbps */
    [CAN_BAUD_125K] = {32, CAN_BS1_6TQ, CAN_BS2_2TQ, CAN_SJW_1TQ}, /* 125Kbps */
    [CAN_BAUD_100K] = {40, CAN_BS1_6TQ, CAN_BS2_2TQ, CAN_SJW_2TQ}, /* 100Kbps */
    [CAN_BAUD_50K]  = {80, CAN_BS1_6TQ, CAN_BS2_2TQ, CAN_SJW_3TQ}, /* 50Kbps */
};

/**
 * @brief 配置CAN过滤器
 */
static void CAN_FilterConfig(CAN_HandleTypeDef *hcan)
{
    CAN_FilterTypeDef filter_config;

    /* 配置过滤器接收所有消息 */
    filter_config.FilterBank           = 0;
    filter_config.FilterMode           = CAN_FILTERMODE_IDMASK;
    filter_config.FilterScale          = CAN_FILTERSCALE_32BIT;
    filter_config.FilterIdHigh         = 0x0000;
    filter_config.FilterIdLow          = 0x0000;
    filter_config.FilterMaskIdHigh     = 0x0000;
    filter_config.FilterMaskIdLow      = 0x0000;
    filter_config.FilterFIFOAssignment = CAN_RX_FIFO0;
    filter_config.FilterActivation     = ENABLE;
    filter_config.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief 初始化CAN设备
 */
void HDL_CAN_Init(CAN_DevID_t dev_id, CAN_BaudRate_t baudrate)
{
    if (dev_id >= CAN_DEV_NUM || baudrate >= sizeof(g_baud_config) / sizeof(g_baud_config[0])) {
        return;
    }

    CAN_Device_t *dev = &g_can_dev_list[dev_id];

    /* 首次初始化队列 */
    if (!dev->is_init) {
        dev->tx_buf = g_can_tx_buf;
        dev->rx_buf = g_can_rx_buf;
        sc_cqueue_create(&dev->tx_queue, dev->tx_buf, CAN_TX_QUEUE_SIZE);
        sc_cqueue_create(&dev->rx_queue, dev->rx_buf, CAN_RX_QUEUE_SIZE);
        dev->hcan = &mhcan; /* STM32F103只有一个CAN外设 */
    }

    /* 配置CAN波特率 */
    const CAN_BaudConfig_t *cfg          = &g_baud_config[baudrate];
    dev->hcan->Instance                  = CAN1;
    dev->hcan->Init.Prescaler            = cfg->prescaler;
    dev->hcan->Init.Mode                 = CAN_MODE_NORMAL;  
    dev->hcan->Init.SyncJumpWidth        = cfg->sjw;
    dev->hcan->Init.TimeSeg1             = cfg->bs1;
    dev->hcan->Init.TimeSeg2             = cfg->bs2;
    dev->hcan->Init.TimeTriggeredMode    = DISABLE;
    dev->hcan->Init.AutoBusOff           = DISABLE;
    dev->hcan->Init.AutoWakeUp           = DISABLE;
    dev->hcan->Init.AutoRetransmission   = ENABLE;
    dev->hcan->Init.ReceiveFifoLocked    = DISABLE;
    dev->hcan->Init.TransmitFifoPriority = DISABLE;

    if (HAL_CAN_Init(dev->hcan) != HAL_OK) {
        Error_Handler();
    }

    /* 配置过滤器 */
    CAN_FilterConfig(dev->hcan);

    /* 启动CAN */
    if (HAL_CAN_Start(dev->hcan) != HAL_OK) {
        Error_Handler();
    }

    /* 使能接收中断 */
    HAL_CAN_ActivateNotification(dev->hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

    /* 禁止发送邮箱空中断 */
    HAL_CAN_DeactivateNotification(dev->hcan, CAN_IT_TX_MAILBOX_EMPTY);

    dev->is_init = true;
}

/**
 * @brief 发送CAN帧
 */
uint8_t HDL_CAN_Write(CAN_DevID_t dev_id, const CAN_Frame_t *frame)
{
    if (dev_id >= CAN_DEV_NUM || frame == NULL || frame->dlc > 8) {
        return 0;
    }

    CAN_Device_t *dev = &g_can_dev_list[dev_id];
    if (!dev->is_init) {
        return 0;
    }

    uint8_t ret = 1;

    DISABLE_INT();

    /* 如果发送邮箱有空闲，直接发送 */
    if (HAL_CAN_GetTxMailboxesFreeLevel(dev->hcan) > 0) {
        /* 填充发送头 */
        CAN_TxHeaderTypeDef tx_header;
        tx_header.StdId              = (frame->id_type == HDL_CAN_ID_STD) ? frame->id & 0x7FFUL : 0;
        tx_header.ExtId              = (frame->id_type == HDL_CAN_ID_EXT) ? frame->id & 0x1FFFFFFUL : 0;
        tx_header.IDE                = (frame->id_type == HDL_CAN_ID_EXT) ? CAN_ID_EXT : CAN_ID_STD;
        tx_header.RTR                = CAN_RTR_DATA;
        tx_header.DLC                = frame->dlc;
        tx_header.TransmitGlobalTime = DISABLE;

        uint32_t mailbox;
        if (HAL_CAN_AddTxMessage(dev->hcan, &tx_header, (uint8_t *)frame->data, &mailbox) == HAL_OK) {
            ret = 0;
        }
    } else {
        // /* 邮箱满了，放入队列 */
        // if (!sc_cqueue_is_full(&dev->tx_queue)) {
        //     sc_cqueue_enqueue(&dev->tx_queue, *frame);
        //     /* 使能发送邮箱空中断，等待邮箱空闲后从队列发送 */
        //     HAL_CAN_ActivateNotification(dev->hcan, CAN_IT_TX_MAILBOX_EMPTY);
        //     ret = 0;
        // }
    }

    ENABLE_INT();

    return ret;
}

/**
 * @brief 接收CAN帧
 */
uint8_t HDL_CAN_Read(CAN_DevID_t dev_id, CAN_Frame_t *frame)
{
    if (dev_id >= CAN_DEV_NUM || frame == NULL) {
        return 0;
    }

    CAN_Device_t *dev = &g_can_dev_list[dev_id];
    if (!dev->is_init) {
        return 0;
    }

    uint8_t ret = 0;
    /* 从接收队列获取数据 */
    DISABLE_INT();
    if (!sc_cqueue_is_empty(&dev->rx_queue)) {
        sc_cqueue_dequeue(&dev->rx_queue, frame);
        ret = 1;
    }
    ENABLE_INT();

    return ret;
}

/**
 * @brief 获取接收队列中可用消息数量
 */
uint16_t HDL_CAN_AvailableMsgNum(CAN_DevID_t dev_id)
{
    if (dev_id >= CAN_DEV_NUM) {
        return 0;
    }

    CAN_Device_t *dev = &g_can_dev_list[dev_id];
    if (!dev->is_init) {
        return 0;
    }

    uint16_t count;
    DISABLE_INT();
    count = sc_cqueue_size(&dev->rx_queue);
    ENABLE_INT();

    return count;
}

/**
 * @brief CAN接收FIFO0消息挂起回调
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    CAN_Frame_t rx_frame;

    /* 接收消息 */
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_frame.data) == HAL_OK) {
        /* 填充帧信息 */
        rx_frame.id      = (rx_header.IDE == CAN_ID_EXT) ? rx_header.ExtId : rx_header.StdId;
        rx_frame.id_type = (rx_header.IDE == CAN_ID_EXT) ? HDL_CAN_ID_EXT : HDL_CAN_ID_STD;
        rx_frame.dlc     = rx_header.DLC;

        /* 加入接收队列 */
        CAN_Device_t *dev = &g_can_dev_list[CAN_DEV_1];
        sc_cqueue_enqueue(&dev->rx_queue, rx_frame);
    }
}

/**
 * @brief CAN发送邮箱空回调
 */
void HAL_CAN_TxMailboxEmptyCallback(CAN_HandleTypeDef *hcan)
{
    CAN_Device_t *dev = &g_can_dev_list[CAN_DEV_1];

    /* 从发送队列取出数据并发送 */
    while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) > 0) {
        CAN_Frame_t *p_tx_frame = NULL;

        if (sc_cqueue_is_empty(&dev->tx_queue)) {
            HAL_CAN_DeactivateNotification(dev->hcan, CAN_IT_TX_MAILBOX_EMPTY);
            break;
        } else {

            p_tx_frame = &sc_cqueue_peek_first(&dev->tx_queue);

            /* 填充发送头 */
            CAN_TxHeaderTypeDef tx_header;
            tx_header.StdId              = (p_tx_frame->id_type == HDL_CAN_ID_STD) ? p_tx_frame->id & 0x7FFUL : 0;
            tx_header.ExtId              = (p_tx_frame->id_type == HDL_CAN_ID_EXT) ? p_tx_frame->id & 0x1FFFFFFUL : 0;
            tx_header.IDE                = (p_tx_frame->id_type == HDL_CAN_ID_EXT) ? CAN_ID_EXT : CAN_ID_STD;
            tx_header.RTR                = CAN_RTR_DATA;
            tx_header.DLC                = p_tx_frame->dlc;
            tx_header.TransmitGlobalTime = DISABLE;

            uint32_t mailbox;
            if (HAL_CAN_AddTxMessage(hcan, &tx_header, p_tx_frame->data, &mailbox) != HAL_OK) {
                Error_Handler();
            }
            /* 无论是否发送成功,从发送队列中删除数据 */
            sc_cqueue_del_first(&dev->tx_queue);
        }
    }
}

/**
 * @brief HAL MSP初始化回调 (从can.c迁移)
 */
void HAL_CAN_MspInit(CAN_HandleTypeDef *canHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (canHandle->Instance == CAN1) {
        /* 使能CAN1时钟 */
        __HAL_RCC_CAN1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /**CAN GPIO配置
         * PA11 ------> CAN_RX
         * PA12 ------> CAN_TX
         */
        GPIO_InitStruct.Pin  = GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin   = GPIO_PIN_12;
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* 使能CAN1中断 */
        HAL_NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);

        HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    }
}

/**
 * @brief HAL MSP反初始化回调 (从can.c迁移)
 */
void HAL_CAN_MspDeInit(CAN_HandleTypeDef *canHandle)
{
    if (canHandle->Instance == CAN1) {
        /* 禁用CAN1时钟 */
        __HAL_RCC_CAN1_CLK_DISABLE();

        /**CAN GPIO去初始化
         * PA11 ------> CAN_RX
         * PA12 ------> CAN_TX
         */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);

        /* 禁用CAN1中断 */
        HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
        HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    }
}

/**
 * @brief This function handles USB low priority or CAN RX0 interrupts.
 */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 0 */

    /* USER CODE END USB_LP_CAN1_RX0_IRQn 0 */
    HAL_CAN_IRQHandler(&mhcan);
    /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 1 */

    /* USER CODE END USB_LP_CAN1_RX0_IRQn 1 */
}

/**
 * @brief This function handles USB high priority or CAN TX interrupts.
 */
void USB_HP_CAN1_TX_IRQHandler(void)
{
    /* USER CODE BEGIN USB_HP_CAN1_TX_IRQn 0 */

    /* USER CODE END USB_HP_CAN1_TX_IRQn 0 */
    HAL_CAN_IRQHandler(&mhcan);
    /* USER CODE BEGIN USB_HP_CAN1_TX_IRQn 1 */

    /* USER CODE END USB_HP_CAN1_TX_IRQn 1 */
}
