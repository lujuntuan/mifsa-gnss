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

#ifndef MIFSA_GNSS_INTERFACE_REALIZE_FDBUS_H
#define MIFSA_GNSS_INTERFACE_REALIZE_FDBUS_H

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
#include "mifsa/gnss/interface.h"

MIFSA_NAMESPACE_BEGIN

int _time_out = 5000;

namespace Gnss {

class FdbusClient : public CBaseClient {
public:
    FdbusClient()
        : CBaseClient("mifsa_gnss_client", &worker)
    {
        worker.start();
    }
    ~FdbusClient()
    {
        worker.flush();
        worker.exit();
        disconnect();
    }
    void onOnline(FDBUS_ONLINE_ARG_TYPE) override
    {
        LOG_DEBUG("onOnline!");
        CFdbMsgSubscribeList subList;
        addNotifyItem(subList, mifsa::gnss::pb::MSG_LOCATION);
        subscribe(subList);
        cbConnected(true);
    }
    void onOffline(FDBUS_ONLINE_ARG_TYPE) override
    {
        LOG_DEBUG("onOffline!");
        cbConnected(false);
    }
    void onBroadcast(CBaseJob::Ptr& msg_ref) override
    {
        auto msg = castToMessage<CBaseMessage*>(msg_ref);
        if (msg->code() == mifsa::gnss::pb::MSG_LOCATION) {
            if (!cbLocation) {
                return;
            }
            mifsa::gnss::pb::Location pb_location;
            if (msg->getPayloadSize() > 0) {
                CFdbProtoMsgParser parser(pb_location);
                if (!msg->deserialize(parser)) {
                    LOG_WARNING("deserialize msg error");
                    return;
                }
            }
            Location location;
            location.size = (int)pb_location.size();
            location.flags = pb_location.flags();
            location.latitude = pb_location.latitude();
            location.longitude = pb_location.longitude();
            location.altitude = pb_location.altitude();
            location.speed = pb_location.speed();
            location.bearing = pb_location.bearing();
            location.accuracy = pb_location.accuracy();
            location.timestamp = pb_location.timestamp();
            location.data = pb_location.data();
            cbLocation(location);
        }
    }

public:
    CBaseWorker worker;
    CbConnected cbConnected;
    CbLocation cbLocation;
};

class InterfaceImplementation : public Interface {
public:
    InterfaceImplementation()
    {
        FDB_CONTEXT->start();
        m_client = std::make_unique<FdbusClient>();
        m_client->cbConnected = _cbConnected;
        m_client->connect("svc://mifsa_gnss");
    }
    virtual std::string version() override
    {
        return MIFSA_GNSS_VERSION;
    }
    virtual bool connected() override
    {
        return m_client->connected();
    }
    virtual std::string getNmea() override
    {
        std::string nmea;
        mifsa::gnss::pb::Command pb_command;
        pb_command.set_type(mifsa::gnss::pb::Command_Type_QUERY_NMEA);
        CFdbProtoMsgBuilder builder(pb_command);
        CBaseJob::Ptr msg_ref(new CBaseMessage(mifsa::gnss::pb::MSG_COMMAND));
        m_client->invoke(msg_ref, builder, _time_out);
        auto msg = castToMessage<CBaseMessage*>(msg_ref);
        mifsa::gnss::pb::Nmea pb_nmea;
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
        m_client->cbLocation = cb;
        mifsa::gnss::pb::Command pb_command;
        pb_command.set_type(mifsa::gnss::pb::Command_Type_START_NAVIGATION);
        CFdbProtoMsgBuilder builder(pb_command);
        m_client->invoke(mifsa::gnss::pb::MSG_COMMAND, builder, _time_out);
    }
    virtual void stopNavigation() override
    {
        mifsa::gnss::pb::Command pb_command;
        pb_command.set_type(mifsa::gnss::pb::Command_Type_STOP_NAVIGATION);
        CFdbProtoMsgBuilder builder(pb_command);
        m_client->invoke(mifsa::gnss::pb::MSG_COMMAND, builder, _time_out);
    }

private:
    std::unique_ptr<FdbusClient> m_client;
};

}

MIFSA_NAMESPACE_END

#endif

#endif // MIFSA_GNSS_INTERFACE_REALIZE_FDBUS_H
