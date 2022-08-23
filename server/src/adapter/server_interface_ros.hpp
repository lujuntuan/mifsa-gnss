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

#ifndef MIFSA_GNSS_SERVER_INTERFACE_ROS_H
#define MIFSA_GNSS_SERVER_INTERFACE_ROS_H

#include "mifsa/gnss/server_interface.h"
#include <mifsa/base/semaphore.h>
#include <mifsa/base/thread.h>
#include <mifsa_gnss_idl/msg/command.hpp>
#include <mifsa_gnss_idl/msg/location.hpp>
#include <mifsa_gnss_idl/srv/nmea.hpp>
#include <rclcpp/rclcpp.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
static mifsa_gnss_idl::msg::Location _getLocation(const Location& location)
{
    mifsa_gnss_idl::msg::Location t_location;
    t_location.size = location.size;
    t_location.flags = location.flags;
    t_location.latitude = location.latitude;
    t_location.longitude = location.longitude;
    t_location.altitude = location.altitude;
    t_location.speed = location.speed;
    t_location.bearing = location.bearing;
    t_location.accuracy = location.accuracy;
    t_location.timestamp = location.timestamp;
    t_location.data = location.data;
    return t_location;
}

class ServertInterfaceAdapter : public ServerInterface {
public:
    ServertInterfaceAdapter()
    {
        Semaphore sema;
        m_thread.start([&]() {
            rclcpp::init(0, nullptr);
            m_node = rclcpp::Node::make_shared("mifsa_gnss_server");
            m_callbackGroup = m_node->create_callback_group(rclcpp::CallbackGroupType::Reentrant);
            auto qosConfig = rclcpp::QoS(rclcpp::KeepLast(10)).reliable();
            m_locationPub = m_node->create_publisher<mifsa_gnss_idl::msg::Location>("/mifsa/gnss/location", qosConfig);
            m_commandSub = m_node->create_subscription<mifsa_gnss_idl::msg::Command>("/mifsa/gnss/command", qosConfig, [this](mifsa_gnss_idl::msg::Command::UniquePtr ros_command) {
                if (ros_command->type == mifsa_gnss_idl::msg::Command::START_NAVIGATION) {
                    if (cbStartNavigation) {
                        cbStartNavigation();
                    }
                } else if (ros_command->type == mifsa_gnss_idl::msg::Command::STOP_NAVIGATION) {
                    if (cbStopNavigation) {
                        cbStopNavigation();
                    }
                }
            });
            m_nmeaService = m_node->create_service<mifsa_gnss_idl::srv::Nmea>(
                "mifsa_gnss_server_nmea", [this](const std::shared_ptr<mifsa_gnss_idl::srv::Nmea::Request> request, std::shared_ptr<mifsa_gnss_idl::srv::Nmea::Response> response) {
                    if (request->type == mifsa_gnss_idl::srv::Nmea::Request::QUERY_NMEA) {
                        std::string nmea;
                        if (cbNmea) {
                            cbNmea(nmea);
                        }
                        response->data = nmea;
                    } else {
                        response->data = "";
                    }
                },
                ::rmw_qos_profile_default, m_callbackGroup);
            sema.reset();
            rclcpp::executors::MultiThreadedExecutor exec;
            exec.add_node(m_node);
            exec.spin();
        });
        sema.acquire();
    }
    ~ServertInterfaceAdapter()
    {
        rclcpp::shutdown();
        m_thread.stop();
    }
    virtual void onStarted() override
    {
    }
    virtual void onStoped() override
    {
    }
    virtual void setCbNmea(const CbNmea& cb) override
    {
        cbNmea = cb;
    }
    virtual void reportGnss(const Location& location) override
    {
        m_locationPub->publish(_getLocation(location));
    }
    virtual void setCbStartNavigation(const CbStartNavigation& cb) override
    {
        cbStartNavigation = cb;
    }
    virtual void setCbStopNavigation(const CbStopNavigation& cb) override
    {
        cbStopNavigation = cb;
    }

private:
    Thread m_thread;
    rclcpp::Node::SharedPtr m_node;
    rclcpp::CallbackGroup::SharedPtr m_callbackGroup;
    rclcpp::Publisher<mifsa_gnss_idl::msg::Location>::SharedPtr m_locationPub;
    rclcpp::Subscription<mifsa_gnss_idl::msg::Command>::SharedPtr m_commandSub;
    rclcpp::Service<mifsa_gnss_idl::srv::Nmea>::SharedPtr m_nmeaService;
    //
    CbNmea cbNmea;
    CbStartNavigation cbStartNavigation;
    CbStopNavigation cbStopNavigation;
};

}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_SERVER_INTERFACE_ROS_H
