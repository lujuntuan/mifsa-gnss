/*********************************************************************************
 *Copyright(C): Juntuan.Lu, 2020-2030, All rights reserved.
 *Author:  Juntuan.Lu
 *Version: 1.0
 *Date:  2022/04/01
 *Email: 931852884@qq.com
 *Description:
 *Others:
 *Function List:
 *History:
 **********************************************************************************/

#ifndef MIFSA_GNSS_TYPES_C_H
#define MIFSA_GNSS_TYPES_C_H

#include <stdbool.h>
#include <stdint.h>
#include <mifsa/base/define.h>

C_INTERFACE_BEGIN

typedef struct mifsa_gnss_location {
    int size;
    int flags;
    double latitude;
    double longitude;
    double altitude;
    float speed;
    float bearing;
    float accuracy;
    int64_t timestamp;
    char* data;
    int data_len;
} mifsa_gnss_location_t;

typedef void (*mifsa_gnss_server_connected_cb_t)(bool connected);
typedef void (*mifsa_gnss_location_cb_t)(const mifsa_gnss_location_t* location);

typedef struct mifsa_gnss_cb_tables {
    mifsa_gnss_server_connected_cb_t connected_cb;
    mifsa_gnss_location_cb_t location_cb;
} mifsa_gnss_cb_tables_t;

C_INTERFACE_END

#endif // MIFSA_GNSS_TYPES_C_H
