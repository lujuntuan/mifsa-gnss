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

#ifndef MIFSA_GNSS_CLIENT_INTERFACE_VSOMEIP_H
#define MIFSA_GNSS_CLIENT_INTERFACE_VSOMEIP_H

#include "mifsa/gnss/client_interface.h"
#include <CommonAPI/CommonAPI.hpp>
#include <mifsa/utils/dir.h>
#include <mifsa/utils/host.h>
#include <v1/commonapi/mifsa_gnss_idlProxy.hpp>

using namespace v1_0::commonapi;

MIFSA_NAMESPACE_BEGIN

CommonAPI::CallInfo _callInfo(5000);

namespace Gnss {
static Location _getLocation(const mifsa_gnss_idl::Location& t_location)
{
    Location location;
    location.size = t_location.getSize();
    location.flags = t_location.getFlags();
    location.latitude = t_location.getLatitude();
    location.longitude = t_location.getLongitude();
    location.altitude = t_location.getAltitude();
    location.speed = t_location.getSpeed();
    location.bearing = t_location.getBearing();
    location.accuracy = t_location.getAccuracy();
    location.timestamp = t_location.getTimestamp();
    location.data = t_location.getData();
    return location;
}

class ClientInterfaceAdapter : public ClientInterface {
public:
    ClientInterfaceAdapter()
    {
        std::string vsomeipApiCfg = Utils::getCfgPath("vsomeip_mifsa_gnss_client.json", "VSOMEIP_CONFIGURATION", "mifsa");
        if (!vsomeipApiCfg.empty()) {
            Utils::setEnvironment("VSOMEIP_CONFIGURATION", vsomeipApiCfg);
        }
        m_commonApiProxy = CommonAPI::Runtime::get()->buildProxy<mifsa_gnss_idlProxy>("local", "commonapi.mifsa.gnss.interfaces", "mifsa_gnss_client");
        m_commonApiProxy->getProxyStatusEvent().subscribe([this](const CommonAPI::AvailabilityStatus& status) {
            if (status == CommonAPI::AvailabilityStatus::AVAILABLE) {
                cbConnected(true);
            } else {
                cbConnected(false);
            }
        });
        m_commonApiProxy->getReportLocationEvent().subscribe([this](const mifsa_gnss_idl::Location& t_location) {
            if (cbLocation) {
                cbLocation(_getLocation(t_location));
            }
        });
    }
    ~ClientInterfaceAdapter()
    {
        m_commonApiProxy.reset();
        CommonAPI::Runtime::get().reset();
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
        return m_commonApiProxy->isAvailable();
    }
    virtual std::string getNmea() override
    {
        CommonAPI::CallStatus status;
        mifsa_gnss_idl::Nmea nmea;
        m_commonApiProxy->getNmea(status, nmea, &_callInfo);
        if (status != CommonAPI::CallStatus::SUCCESS) {
            LOG_WARNING("invoke failed, error code=", (int)status);
        }
        return nmea.getData();
    }
    virtual void startNavigation(const CbLocation& cb) override
    {
        cbLocation = cb;
        CommonAPI::CallStatus status;
        m_commonApiProxy->startNavigation(status);
        if (status != CommonAPI::CallStatus::SUCCESS) {
            LOG_WARNING("invoke failed, error code=", (int)status);
        }
    }
    virtual void stopNavigation() override
    {
        CommonAPI::CallStatus status;
        m_commonApiProxy->stopNavigation(status);
        if (status != CommonAPI::CallStatus::SUCCESS) {
            LOG_WARNING("invoke failed, error code=", (int)status);
        }
    }

private:
    std::shared_ptr<mifsa_gnss_idlProxy<>> m_commonApiProxy;
    CbLocation cbLocation;
};

}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_CLIENT_INTERFACE_VSOMEIP_H
