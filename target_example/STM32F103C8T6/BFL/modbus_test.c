#include "modbus_test.h"
#include "HDL_CPU_Time.h"
#include "log.h"
#include "main.h"
#include "test.h"
#include "mtime.h"

//TODO:修改了modbus的接口后有些测试已经不能用来
void modbus_rtu_host_test()
{
    HDL_CPU_Time_Init();
    modbus_rtu_host_init(hModbusRTU3, 115200, 'N', 8);
    ulog_init_user();
    uint16_t var = 1;
    while (1)
    {
        modbus_rtu_handler();
        // 一秒执行一次
        if (period_query(0, 500))
        {
            var++;
            if (modbus_rtu_host_read_03H(hModbusRTU3, 0x01, 0x0001, 1) == 1)
            {
                ULOG_DEBUG("Read 03H reg %x receive %d bytes %d\r\n", 0x0001, hModbusRTU3->RxCount, 1);
                for (size_t i = 0; i < hModbusRTU3->RxCount; i++)
                {
                    ULOG_DEBUG("%#x ", hModbusRTU3->RxBuf[i]);
                }
                ULOG_DEBUG("\r\n\r\n");
            }
        }
    }
}
