#include "SimpleDPP.h"
#include <stdio.h>
#include <string.h>
__implemented sdp_byte __send_data[SIMPLEDDP_MAX_FRAME_ENCODED_SIZE];
__implemented sdp_byte __recv_data[SIMPLEDDP_MAX_FRAME_ENCODED_SIZE];
SimpleDPP sdp;

__implemented void SimpleDPPRecvCallback(const sdp_byte *data, int len)
{
    printf("SimpleDPPRecvCallback------------------> \r\n");
    // print data
    int i = 0;
    for (i; i < len; i++)
    {
        putchar(data[i]);
    }
}
__implemented size_t SimpleDPP_write(const sdp_byte *data, int len)
{
    for (size_t i = 0; i < len; i++)
    {
        SimpleDPP_parse(&sdp, data[i]);
        putchar(data[i]);
    }

    return len;
}

__implemented void SimpleDPPRevErrorCallback(SimpleDPPERROR error_code)
{
}

int main(void)
{

    // 1. Init Simple DPP data buffer and callback,if you don't want to use callback,you can set callback to NULL.
    SimpleDPP_Constructor(&sdp,
                          __send_data, SIMPLEDDP_MAX_FRAME_ENCODED_SIZE,
                          __recv_data, SIMPLEDDP_MAX_FRAME_ENCODED_SIZE,
                          SimpleDPPRecvCallback,
                          SimpleDPPRevErrorCallback,
                          SimpleDPP_write);

    // 2. send and parse one msg,msg cnn be type of char * or byte *
    char *msg = "hello worl@\\00\r\n000d";
    if (SimpleDPP_send(&sdp, msg, strlen(msg)) == SIMPLEDPP_SENDFAILED)
    {
        printf("send error\n");
    }

    // 3. send and parse multiple msg,msg cnn be type of char * or byte *
    if (SimpleDPP_send_datas(&sdp, "hello", 5, "AA", 2) == SIMPLEDPP_SENDFAILED)
    {
        printf("send error\n");
    }

    return 0;
}
