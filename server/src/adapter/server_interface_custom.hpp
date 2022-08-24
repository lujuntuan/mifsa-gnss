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

#ifndef MIFSA_GNSS_SERVER_INTERFACE_CUSTOM_H
#define MIFSA_GNSS_SERVER_INTERFACE_CUSTOM_H

#include "mifsa/gnss/server_interface.h"

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
class ServertInterfaceAdapter : public ServerInterface {
public:
    ServertInterfaceAdapter()
    {
        LOG_WARNING("Rpc adaptation layer is not implemented!");
    }
    ~ServertInterfaceAdapter()
    {
    }
    virtual void onStarted() override
    {
    }
    virtual void onStoped() override
    {
    }
    virtual void setCbNmea(const CbNmea& cb) override
    {
    }
    virtual void reportGnss(const Location& location) override
    {
    }
    virtual void setCbStartNavigation(const CbStartNavigation& cb) override
    {
    }
    virtual void setCbStopNavigation(const CbStopNavigation& cb) override
    {
    }
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_SERVER_INTERFACE_CUSTOM_H
