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

#ifndef MIFSA_GNSS_PROVIDER_H
#define MIFSA_GNSS_PROVIDER_H

#include "types.h"
#include <mifsa/module/provider.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
class Provider : public ProviderBase {
public:
    virtual void setCbNmea(const CbNmea& cb) = 0;
    virtual void reportGnss(const Location& location) = 0;
    virtual void setCbStartNavigation(const CbStartNavigation& cb) = 0;
    virtual void setCbStopNavigation(const CbStopNavigation& cb) = 0;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_PROVIDER_H
