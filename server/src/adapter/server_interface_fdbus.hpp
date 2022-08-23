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

#ifndef MIFSA_GNSS_SERVER_INTERFACE_FDBUS_H
#define MIFSA_GNSS_SERVER_INTERFACE_FDBUS_H

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
#include "mifsa/gnss/server_interface.h"
#include <mifsa/base/thread.h>

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
static mifsa_gnss_idl::Location _getLocation(const Location& location)
{
    mifsa_gnss_idl::Location t_location;
    t_location.set_size(location.size);
    t_location.set_flags(location.flags);
    t_location.set_latitude(location.latitude);
    t_location.set_longitude(location.longitude);
    t_location.set_altitude(location.altitude);
    t_location.set_speed(location.speed);
    t_location.set_bearing(location.bearing);
    t_location.set_accuracy(location.accuracy);
    t_location.set_timestamp(location.timestamp);
    t_location.set_data(location.data);
    return t_location;
}

class ServertInterfaceAdapter : public ServerInterface, public CBaseServer {
public:
    ServertInterfaceAdapter()
        : CBaseServer("mifsa_gnss_client", &m_worker)
    {
        FDB_CONTEXT->start();
        m_worker.start();
        CBaseServer::bind("svc://mifsa_gnss");
    }
    ~ServertInterfaceAdapter()
    {
        m_worker.flush();
        m_worker.exit();
        CBaseServer::unbind();
        CBaseServer::disconnect();
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
        CFdbProtoMsgBuilder builder(_getLocation(location));
        CBaseServer::broadcast(mifsa_gnss_idl::MSG_LOCATION, builder);
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
    void onOnline(FDBUS_ONLINE_ARG_TYPE) override
    {
    }
    void onOffline(FDBUS_ONLINE_ARG_TYPE) override
    {
    }
    void onSubscribe(CBaseJob::Ptr& msg_ref) override
    {
    }
    void onInvoke(CBaseJob::Ptr& msg_ref) override
    {
        auto msg = castToMessage<CBaseMessage*>(msg_ref);
        if (msg->code() == mifsa_gnss_idl::MSG_COMMAND) {
            mifsa_gnss_idl::Command pb_command;
            if (msg->getPayloadSize() > 0) {
                CFdbProtoMsgParser parser(pb_command);
                if (!msg->deserialize(parser)) {
                    LOG_WARNING("deserialize msg error");
                    return;
                }
            }
            if (pb_command.type() == mifsa_gnss_idl::Command_Type_QUERY_NMEA) {
                std::string nmea;
                if (m_cbNmea) {
                    m_cbNmea(nmea);
                }
                mifsa_gnss_idl::Nmea pb_nmea;
                pb_nmea.set_data(nmea);
                CFdbProtoMsgBuilder builder(pb_nmea);
                msg->reply(msg_ref, builder);
            } else if (pb_command.type() == mifsa_gnss_idl::Command_Type_START_NAVIGATION) {
                if (m_cbStartNavigation) {
                    m_cbStartNavigation();
                }
            } else if (pb_command.type() == mifsa_gnss_idl::Command_Type_STOP_NAVIGATION) {
                if (m_cbStopNavigation) {
                    m_cbStopNavigation();
                }
            }
        }
    }

private:
    CBaseWorker m_worker;
    ServerInterface::CbNmea m_cbNmea;
    ServerInterface::CbStartNavigation m_cbStartNavigation;
    ServerInterface::CbStopNavigation m_cbStopNavigation;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_SERVER_INTERFACE_FDBUS_H
