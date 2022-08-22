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

#ifdef MIFSA_SUPPORT_VSOMEIP

#include "mifsa/gnss/server_interface.h"
#include <CommonAPI/CommonAPI.hpp>
#include <mifsa/utils/dir.h>
#include <mifsa/utils/host.h>
#include <v1/commonapi/mifsa/gnss/interfacesStubDefault.hpp>

using namespace v1_0::commonapi::mifsa::gnss;

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
class ServertInterfaceAdapter : public ServerInterface, protected interfacesStubDefault {
public:
    ServertInterfaceAdapter()
    {
        std::string vsomeipApiCfg = Utils::getCfgPath("vsomeip_mifsa_gnss_server.json", "VSOMEIP_CONFIGURATION", "mifsa");
        if (!vsomeipApiCfg.empty()) {
            Utils::setEnvironment("VSOMEIP_CONFIGURATION", vsomeipApiCfg);
        }
        std::shared_ptr<interfacesStubDefault> ptr = std::shared_ptr<interfacesStubDefault>((interfacesStubDefault*)this);
        CommonAPI::Runtime::get()->registerService("local", "commonapi.mifsa.gnss.interfaces", ptr, "mifsa_gnss_server");
    }
    ~ServertInterfaceAdapter()
    {
        CommonAPI::Runtime::get().reset();
    }
    virtual void setCbNmea(const CbNmea& cb) override
    {
        m_cbNmea = cb;
    }
    virtual void reportGnss(const Location& location) override
    {
        interfaces::Location capi_location;
        capi_location.setSize(location.size);
        capi_location.setFlags(location.flags);
        capi_location.setLatitude(location.latitude);
        capi_location.setLongitude(location.longitude);
        capi_location.setAltitude(location.altitude);
        capi_location.setSpeed(location.speed);
        capi_location.setBearing(location.bearing);
        capi_location.setAccuracy(location.accuracy);
        capi_location.setTimestamp(location.timestamp);
        capi_location.setData(location.data);
        interfacesStubDefault::fireReportLocationEvent(capi_location);
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

#endif

#endif // MIFSA_GNSS_SERVER_INTERFACE_VSOMEIP_H
