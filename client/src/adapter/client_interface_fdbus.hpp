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
        mifsa::gnss::pb::Command pb_command;
        pb_command.set_type(mifsa::gnss::pb::Command_Type_QUERY_NMEA);
        CFdbProtoMsgBuilder builder(pb_command);
        CBaseJob::Ptr msg_ref(new CBaseMessage(mifsa::gnss::pb::MSG_COMMAND));
        CBaseClient::invoke(msg_ref, builder, _time_out);
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
        m_cbLocation = cb;
        mifsa::gnss::pb::Command pb_command;
        pb_command.set_type(mifsa::gnss::pb::Command_Type_START_NAVIGATION);
        CFdbProtoMsgBuilder builder(pb_command);
        CBaseClient::invoke(mifsa::gnss::pb::MSG_COMMAND, builder, _time_out);
    }
    virtual void stopNavigation() override
    {
        mifsa::gnss::pb::Command pb_command;
        pb_command.set_type(mifsa::gnss::pb::Command_Type_STOP_NAVIGATION);
        CFdbProtoMsgBuilder builder(pb_command);
        CBaseClient::invoke(mifsa::gnss::pb::MSG_COMMAND, builder, _time_out);
    }

protected:
    void onOnline(FDBUS_ONLINE_ARG_TYPE) override
    {
        CFdbMsgSubscribeList subList;
        CBaseClient::addNotifyItem(subList, mifsa::gnss::pb::MSG_LOCATION);
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
        if (msg->code() == mifsa::gnss::pb::MSG_LOCATION) {
            if (!m_cbLocation) {
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
            m_cbLocation(location);
        }
    }

private:
    CBaseWorker m_worker;
    ClientInterface::CbLocation m_cbLocation;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_CLIENT_INTERFACE_FDBUS_H
