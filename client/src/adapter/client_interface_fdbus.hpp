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

#ifndef MIFSA_GNSS_CLIENT_INTERFACE_FDBUS_H
#define MIFSA_GNSS_CLIENT_INTERFACE_FDBUS_H

#ifdef WIN32
#ifndef __WIN32__
#define __WIN32__
#endif
#endif

#ifdef FDBUS_NEW_INTERFACE
#define FDBUS_ONLINE_ARG_TYPE const CFdbOnlineInfo&
#include <fdbus/CFdbProtoMsgBuilder.h>
#include <fdbus/fdbus.h>
using namespace ipc::fdbus;
#else
#define FDBUS_ONLINE_ARG_TYPE FdbSessionId_t, bool
#include <common_base/CFdbProtoMsgBuilder.h>
#include <common_base/fdbus.h>
#endif

#include "mifsa/gnss/client_interface.h"
#include "mifsa/gnss/idl/fdbus/gnss.pb.h"

MIFSA_NAMESPACE_BEGIN

int _time_out = 5000;

namespace Gnss {
static Location _getLocation(const mifsa_gnss_idl::Location& t_location)
{
    Location location;
    location.size = (int)t_location.size();
    location.flags = t_location.flags();
    location.latitude = t_location.latitude();
    location.longitude = t_location.longitude();
    location.altitude = t_location.altitude();
    location.speed = t_location.speed();
    location.bearing = t_location.bearing();
    location.accuracy = t_location.accuracy();
    location.timestamp = t_location.timestamp();
    location.data = t_location.data();
    return location;
}

class ClientInterfaceAdapter : public ClientInterface, protected CBaseClient {
public:
    ClientInterfaceAdapter()
        : CBaseClient("mifsa_gnss_client", &m_worker)
    {
        FDB_CONTEXT->start();
        m_worker.start();
        CBaseClient::connect("svc://mifsa_gnss");
    }
    ~ClientInterfaceAdapter()
    {
        m_worker.flush();
        m_worker.exit();
        CBaseClient::disconnect();
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
        return CBaseClient::connected();
    }
    virtual std::string getNmea() override
    {
        std::string nmea;
        mifsa_gnss_idl::Command pb_command;
        pb_command.set_type(mifsa_gnss_idl::Command_Type_QUERY_NMEA);
        CFdbProtoMsgBuilder builder(pb_command);
        CBaseJob::Ptr msg_ref(new CBaseMessage(mifsa_gnss_idl::MSG_COMMAND));
        CBaseClient::invoke(msg_ref, builder, _time_out);
        auto msg = castToMessage<CBaseMessage*>(msg_ref);
        mifsa_gnss_idl::Nmea pb_nmea;
        if (msg->getPayloadSize() > 0) {
            CFdbProtoMsgParser parser(pb_nmea);
            if (!msg->deserialize(parser)) {
                LOG_WARNING("deserialize msg error");
                return nmea;
            }
        }
        nmea = pb_nmea.data();
        return nmea;
    }
    virtual void startNavigation(const CbLocation& cb) override
    {
        m_cbLocation = cb;
        mifsa_gnss_idl::Command pb_command;
        pb_command.set_type(mifsa_gnss_idl::Command_Type_START_NAVIGATION);
        CFdbProtoMsgBuilder builder(pb_command);
        CBaseClient::invoke(mifsa_gnss_idl::MSG_COMMAND, builder, _time_out);
    }
    virtual void stopNavigation() override
    {
        mifsa_gnss_idl::Command pb_command;
        pb_command.set_type(mifsa_gnss_idl::Command_Type_STOP_NAVIGATION);
        CFdbProtoMsgBuilder builder(pb_command);
        CBaseClient::invoke(mifsa_gnss_idl::MSG_COMMAND, builder, _time_out);
    }

protected:
    void onOnline(FDBUS_ONLINE_ARG_TYPE) override
    {
        CFdbMsgSubscribeList subList;
        CBaseClient::addNotifyItem(subList, mifsa_gnss_idl::MSG_LOCATION);
        CBaseClient::subscribe(subList);
        cbConnected(true);
    }
    void onOffline(FDBUS_ONLINE_ARG_TYPE) override
    {
        cbConnected(false);
    }
    void onBroadcast(CBaseJob::Ptr& msg_ref) override
    {
        auto msg = castToMessage<CBaseMessage*>(msg_ref);
        if (msg->code() == mifsa_gnss_idl::MSG_LOCATION) {
            if (!m_cbLocation) {
                return;
            }
            mifsa_gnss_idl::Location t_location;
            if (msg->getPayloadSize() > 0) {
                CFdbProtoMsgParser parser(t_location);
                if (!msg->deserialize(parser)) {
                    LOG_WARNING("deserialize msg error");
                    return;
                }
            }
            m_cbLocation(_getLocation(t_location));
        }
    }

private:
    CBaseWorker m_worker;
    ClientInterface::CbLocation m_cbLocation;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_CLIENT_INTERFACE_FDBUS_H
