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

#ifndef MIFSA_GNSS_SERVER_INTERFACE_VSOMEIP_H
#define MIFSA_GNSS_SERVER_INTERFACE_VSOMEIP_H

#include "mifsa/gnss/server_interface.h"
#include <CommonAPI/CommonAPI.hpp>
#include <mifsa/utils/dir.h>
#include <mifsa/utils/host.h>
#include <v1/commonapi/mifsa_gnss_idlStubDefault.hpp>

using namespace v1_0::commonapi;

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
static mifsa_gnss_idl::Location _getLocation(const Location& location)
{
    mifsa_gnss_idl::Location t_location;
    t_location.setSize(location.size);
    t_location.setFlags(location.flags);
    t_location.setLatitude(location.latitude);
    t_location.setLongitude(location.longitude);
    t_location.setAltitude(location.altitude);
    t_location.setSpeed(location.speed);
    t_location.setBearing(location.bearing);
    t_location.setAccuracy(location.accuracy);
    t_location.setTimestamp(location.timestamp);
    t_location.setData(location.data);
    return t_location;
}

class ServertInterfaceAdapter : public ServerInterface, protected mifsa_gnss_idlStubDefault {
public:
    ServertInterfaceAdapter()
    {
        std::string vsomeipApiCfg = Utils::getCfgPath("vsomeip_mifsa_gnss_server.json", "VSOMEIP_CONFIGURATION", "mifsa");
        if (!vsomeipApiCfg.empty()) {
            Utils::setEnvironment("VSOMEIP_CONFIGURATION", vsomeipApiCfg);
        }
        std::shared_ptr<mifsa_gnss_idlStubDefault> ptr = std::shared_ptr<mifsa_gnss_idlStubDefault>((mifsa_gnss_idlStubDefault*)this);
        CommonAPI::Runtime::get()->registerService("local", "commonapi.mifsa.gnss.interfaces", ptr, "mifsa_gnss_server");
    }
    ~ServertInterfaceAdapter()
    {
        CommonAPI::Runtime::get().reset();
    }
    virtual void onStarted() override
    {
    }
    virtual void onStoped() override
    {
    }
    virtual void setCbNmea(const CbNmea& cb) override
    {
        m_cbNmea = cb;
    }
    virtual void reportGnss(const Location& location) override
    {
        mifsa_gnss_idlStubDefault::fireReportLocationEvent(_getLocation(location));
    }
    virtual void setCbStartNavigation(const CbStartNavigation& cb) override
    {
        m_cbStartNavigation = cb;
    }
    virtual void setCbStopNavigation(const CbStopNavigation& cb) override
    {
        m_cbStopNavigation = cb;
    }

protected:
    virtual void startNavigation(const std::shared_ptr<CommonAPI::ClientId> _client) override
    {
        if (m_cbStartNavigation) {
            m_cbStartNavigation();
        }
    }
    virtual void stopNavigation(const std::shared_ptr<CommonAPI::ClientId> _client) override
    {
        if (m_cbStopNavigation) {
            m_cbStopNavigation();
        }
    }
    virtual void getNmea(const std::shared_ptr<CommonAPI::ClientId> _client, getNmeaReply_t _reply) override
    {
        std::string nmea;
        if (m_cbNmea) {
            m_cbNmea(nmea);
        }
        _reply(nmea);
    }

private:
    ServerInterface::CbNmea m_cbNmea;
    ServerInterface::CbStartNavigation m_cbStartNavigation;
    ServerInterface::CbStopNavigation m_cbStopNavigation;
};

}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_SERVER_INTERFACE_VSOMEIP_H
