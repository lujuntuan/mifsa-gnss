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

#ifndef MIFSA_GNSS_CIENT_INTERFACE_H
#define MIFSA_GNSS_CIENT_INTERFACE_H

#include "common_types.h"
#include <mifsa/module/client.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
class ClientInterface : public ClientInterfaceBase {
public:
    using CbLocation = std::function<void(const Location&)>;
    virtual std::string getNmea() = 0;
    virtual void startNavigation(const CbLocation& cb) = 0;
    virtual void stopNavigation() = 0;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_CIENT_INTERFACE_H
