#include "log.h"
#include "mtime.h"
#include <stdio.h>
#include <stdarg.h>
#ifdef _WIN32
// Windows-specific code
#elif defined(__linux__)
// Linux-specific code
#elif defined(__APPLE__)
// macOS-specific code
#else
#include "HDL_CPU_Time.h"
#include "HDL_Uart.h"
#include "HDL_RTC.h"
// Other systems
#undef FREE_RTOS

#ifdef FREE_RTOS
#include "FreeRTOS.h"
#include "task.h"
#endif // FREE_RTOS
#endif

// 调试输出数据包最大长度
#define MAXDEBUGSEND 256
static uint8_t buffer[MAXDEBUGSEND + 1];
#define DEBUG_COM COM2

/*
    Refference:
    mtime_t mtime;
    uint64_t timestamp = HDL_RTC_GetTimeTick(&mtime.wSub);
    timestamp += 8 * 3600;
    mtime_unix_sec_2_time(timestamp, &mtime);
    SEGGER_RTT_printf(0, "%04d.%02d.%02d %02d:%02d:%02d  [%s]: %s\n",
                        mtime.nYear, mtime.nMonth, mtime.nDay, mtime.nHour, mtime.nMin, mtime.nSec,
                        ulog_level_name(severity),
                        msg);
*/

/**
 * @brief 指定终端的调试格式化输出方法。
 *
 * @param terminal_id 终端ID，仅在RTT模式下有效
 * @param format 格式化字符串
 * @param ... 可变参数
 */
void Debug_Printf_Terminal(uint8_t terminal_id, const void *format, ...)
{

    uint32_t uLen;
    va_list vArgs;
    va_start(vArgs, format);
    uLen = vsnprintf((char *)buffer, MAXDEBUGSEND, (char const *)format, vArgs);
    va_end(vArgs);
    if (uLen > MAXDEBUGSEND)
        uLen = MAXDEBUGSEND;
#if USING_RTT == 1
    SEGGER_RTT_Write(terminal_id, buffer, uLen);
#elif USING_USB_CDC == 1
    UNUSED(terminal_id);
    CDC_Transmit_FS(buffer, uLen);
#elif USING_UART == 1
    UNUSED(terminal_id);
    Uart_Write(DEBUG_COM, buffer, uLen);
#elif USING_SYS == 1
    UNUSED(terminal_id);
    printf("%s", buffer);
#endif // USING_RTT
#ifdef ULOG_ENABLED
#endif // ULOG_ENABLED
}

void Debug_Printf(const void *format, ...)
{
#ifdef ULOG_ENABLED
    uint32_t uLen;
    va_list vArgs;
    va_start(vArgs, format);
    uLen = vsnprintf((char *)buffer, MAXDEBUGSEND, (char const *)format, vArgs);
    va_end(vArgs);
    if (uLen > MAXDEBUGSEND)
        uLen = MAXDEBUGSEND;
#if USING_RTT == 1
    SEGGER_RTT_Write(0, buffer, uLen);
#elif USING_USB_CDC == 1
    CDC_Transmit_FS(buffer, uLen);
#elif USING_UART == 1
    Uart_Write(DEBUG_COM, buffer, uLen);
#elif USING_SYS == 1
    printf("%s", buffer);
#endif
#endif // ULOG_ENABLED
}

void my_console_logger(ulog_level_t severity, const char *msg)
{
#ifdef ULOG_ENABLED
#ifdef FREE_RTOS
    taskENTER_CRITICAL();
#endif // FREE_RTOS
    switch (severity) {
        case ULOG_WARNING_LEVEL:
            Debug_Printf(RTT_CTRL_TEXT_YELLOW "%u  [%s]: %s\n" RTT_CTRL_RESET,
                         HDL_CPU_Time_GetTick(),
                         ulog_level_name(severity),
                         msg);
            break;
        case ULOG_ERROR_LEVEL:
            Debug_Printf(RTT_CTRL_TEXT_RED "%u  [%s]: %s\n" RTT_CTRL_RESET,
                         HDL_CPU_Time_GetTick(),
                         ulog_level_name(severity),
                         msg);
            break;
        case ULOG_CRITICAL_LEVEL:
            Debug_Printf(RTT_CTRL_BG_RED "%u  [%s]: %s\n" RTT_CTRL_RESET,
                         HDL_CPU_Time_GetTick(),
                         ulog_level_name(severity),
                         msg);
            break;
        default:
            Debug_Printf("%u  [%s]: %s\n",
                         HDL_CPU_Time_GetTick(),
                         ulog_level_name(severity),
                         msg);
            break;
    }
#ifdef FREE_RTOS
    taskEXIT_CRITICAL();
#endif // FREE_RTOS
#endif // ULOG_ENABLED
}

void ulog_init_user()
{
#ifdef ULOG_ENABLED
    ULOG_INIT();

#if USING_RTT == 1
   /* 配置通道 0，上行配置 */
   SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
   /* 配置通道 0，下行配置 */
   // SEGGER_RTT_ConfigDownBuffer(0, "RTTDOWN", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
   SEGGER_RTT_SetTerminal(0);
#elif USING_USB_CDC == 1
#elif USING_UART == 1
    Uart_Init(DEBUG_COM, 1500000, LL_USART_DATAWIDTH_8B, LL_USART_STOPBITS_1, LL_USART_PARITY_NONE);
#elif USING_SYS == 1
    // do nothing
#endif
    // dynamically change the threshold for a specific logger
    ULOG_SUBSCRIBE(my_console_logger, ULOG_DEBUG_LEVEL);
#endif // ULOG_ENABLED
}
