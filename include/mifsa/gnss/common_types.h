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

#ifndef MIFSA_GNSS_COMMON_TYPES_H
#define MIFSA_GNSS_COMMON_TYPES_H

#include "mifsa/gnss/config.h"
#include <functional>
#include <mifsa/base/define.h>
#include <string>

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
struct Location {
    int size = 0;
    int flags = 0;
    double latitude = 0;
    double longitude = 0;
    double altitude = 0;
    float speed = 0;
    float bearing = 0;
    float accuracy = 0;
    int64_t timestamp = 0;
    std::string data;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_COMMON_TYPES_H
