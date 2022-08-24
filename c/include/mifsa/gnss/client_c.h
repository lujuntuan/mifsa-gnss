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

#ifndef MIFSA_GNSS_CLIENT_C_H
#define MIFSA_GNSS_CLIENT_C_H

#include "types/location_c.h"
#include <mifsa/base/define.h>

C_INTERFACE_BEGIN

extern MIFSA_EXPORT int mifsa_gnss_init(mifsa_gnss_cb_tables_t cbs);
extern MIFSA_EXPORT int mifsa_gnss_deinit();
extern MIFSA_EXPORT int mifsa_gnss_waitfor_server(int timeout_ms);
extern MIFSA_EXPORT int mifsa_gnss_get_nmea_data(char** nmea, int* size);
extern MIFSA_EXPORT int mifsa_gnss_start_navigation();
extern MIFSA_EXPORT int mifsa_gnss_stop_navigation();

C_INTERFACE_END

#endif // MIFSA_GNSS_CLIENT_C_H
