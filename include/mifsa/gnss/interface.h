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

#ifndef MIFSA_GNSS_INTERFACE_H
#define MIFSA_GNSS_INTERFACE_H

#include "types.h"
#include <mifsa/module/interface.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
class Interface : public InterfaceBase {

public:
    virtual std::string getNmea() = 0;
    virtual void startNavigation(const CbLocation& cb) = 0;
    virtual void stopNavigation() = 0;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_INTERFACE_H
