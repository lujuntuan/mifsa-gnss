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

#ifndef MIFSA_GNSS_PROVIDER_REALIZE_FDBUS_H
#define MIFSA_GNSS_PROVIDER_REALIZE_FDBUS_H

#ifdef MIFSA_SUPPORT_FDBUS

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

#include "mifsa/gnss/idl/fdbus/gnss.pb.h"
#include "mifsa/gnss/provider.h"
#include <mifsa/base/thread.h>

MIFSA_NAMESPACE_BEGIN

namespace Gnss {

class FdbusServer : public CBaseServer {
public:
    FdbusServer()
        : CBaseServer("mifsa_gnss_client", &worker)
    {
        worker.start();
    }
    ~FdbusServer()
    {
        worker.flush();
        worker.exit();
        unbind();
        disconnect();
    }
    void onOnline(FDBUS_ONLINE_ARG_TYPE) override
    {
        LOG_DEBUG("onOnline!");
    }
    void onOffline(FDBUS_ONLINE_ARG_TYPE) override
    {
        LOG_DEBUG("onOffline!");
    }
    void onSubscribe(CBaseJob::Ptr& msg_ref) override
    {
        auto msg = castToMessage<CBaseMessage*>(msg_ref);
        (void)msg;
    }
    void onInvoke(CBaseJob::Ptr& msg_ref) override
    {
        auto msg = castToMessage<CBaseMessage*>(msg_ref);
        if (msg->code() == mifsa::gnss::pb::MSG_COMMAND) {
            mifsa::gnss::pb::Command pb_command;
            if (msg->getPayloadSize() > 0) {
                CFdbProtoMsgParser parser(pb_command);
                if (!msg->deserialize(parser)) {
                    LOG_WARNING("deserialize msg error");
                    return;
                }
            }
            if (pb_command.type() == mifsa::gnss::pb::Command_Type_QUERY_NMEA) {
                std::string nmea;
                if (cbNmea) {
                    cbNmea(nmea);
                }
                mifsa::gnss::pb::Nmea pb_nmea;
                pb_nmea.set_data(nmea);
                CFdbProtoMsgBuilder builder(pb_nmea);
                msg->reply(msg_ref, builder);
            } else if (pb_command.type() == mifsa::gnss::pb::Command_Type_START_NAVIGATION) {
                if (cbStartNavigation) {
                    cbStartNavigation();
                }
            } else if (pb_command.type() == mifsa::gnss::pb::Command_Type_STOP_NAVIGATION) {
                if (cbStopNavigation) {
                    cbStopNavigation();
                }
            }
        }
    }

public:
    CBaseWorker worker;
    CbNmea cbNmea;
    CbStartNavigation cbStartNavigation;
    CbStopNavigation cbStopNavigation;
};

class ProviderImplementation : public Provider {
public:
    ProviderImplementation()
    {
        FDB_CONTEXT->start();
        m_server = std::make_unique<FdbusServer>();
        m_server->bind("svc://mifsa_gnss");
    }
    virtual void setCbNmea(const CbNmea& cb) override
    {
        m_server->cbNmea = cb;
    }
    virtual void reportGnss(const Location& location) override
    {
        mifsa::gnss::pb::Location pb_location;
        pb_location.set_size(location.size);
        pb_location.set_flags(location.flags);
        pb_location.set_latitude(location.latitude);
        pb_location.set_longitude(location.longitude);
        pb_location.set_altitude(location.altitude);
        pb_location.set_speed(location.speed);
        pb_location.set_bearing(location.bearing);
        pb_location.set_accuracy(location.accuracy);
        pb_location.set_timestamp(location.timestamp);
        pb_location.set_data(location.data);
        CFdbProtoMsgBuilder builder(pb_location);
        m_server->broadcast(mifsa::gnss::pb::MSG_LOCATION, builder);
    }
    virtual void setCbStartNavigation(const CbStartNavigation& cb) override
    {
        m_server->cbStartNavigation = cb;
    }
    virtual void setCbStopNavigation(const CbStopNavigation& cb) override
    {
        m_server->cbStopNavigation = cb;
    }

private:
    std::unique_ptr<FdbusServer> m_server;
};

}

MIFSA_NAMESPACE_END

#endif

#endif // MIFSA_GNSS_PROVIDER_REALIZE_FDBUS_H
