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

#ifndef MIFSA_GNSS_CLIENT_INTERFACE_CUSTOM_H
#define MIFSA_GNSS_CLIENT_INTERFACE_CUSTOM_H

#include "mifsa/gnss/client_interface.h"
#include "mifsa/gnss/config.h"

MIFSA_NAMESPACE_BEGIN

int _time_out = 5000;

namespace Gnss {
class ClientInterfaceAdapter : public ClientInterface {
public:
    ClientInterfaceAdapter()
    {
        LOG_WARNING("Rpc adaptation layer is not implemented!");
    }
    ~ClientInterfaceAdapter()
    {
    }
    virtual void onStarted() override
    {
    }
    virtual void onStoped() override
    {
    }
    virtual std::string version() override
    {
        return MIFSA_GNSS_VERSION;
    }
    virtual bool connected() override
    {
        return false;
    }
    virtual std::string getNmea() override
    {
        return std::string();
    }
    virtual void startNavigation(const CbLocation& cb) override
    {
    }
    virtual void stopNavigation() override
    {
    }
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_CLIENT_INTERFACE_CUSTOM_H
