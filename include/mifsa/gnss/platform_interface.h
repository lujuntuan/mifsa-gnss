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

#ifndef MIFSA_GNSS_PLATFORM_H
#define MIFSA_GNSS_PLATFORM_H

#include <mifsa/module/platform.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
class PlatformInterface : public PlatformInterfaceBase {
    MIFSA_PLUGIN_REGISTER("mifsa_gnss_platform")
public:
    virtual std::string getNmea() = 0;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_PLATFORM_H
