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

#ifndef MIFSA_GNSS_CLIENT_INTERFACE_ROS_H
#define MIFSA_GNSS_CLIENT_INTERFACE_ROS_H

#include "mifsa/gnss/client_interface.h"
#include <mifsa/base/thread.h>
#include <mifsa_gnss_idl/msg/command.hpp>
#include <mifsa_gnss_idl/msg/location.hpp>
#include <mifsa_gnss_idl/srv/nmea.hpp>
#include <rclcpp/rclcpp.hpp>

MIFSA_NAMESPACE_BEGIN

int _time_out = 5000;

namespace Gnss {
static Location _getLocation(const mifsa_gnss_idl::msg::Location::UniquePtr& t_location)
{
    Location location;
    location.size = t_location->size;
    location.flags = t_location->flags;
    location.latitude = t_location->latitude;
    location.longitude = t_location->longitude;
    location.altitude = t_location->altitude;
    location.speed = t_location->speed;
    location.bearing = t_location->bearing;
    location.accuracy = t_location->accuracy;
    location.timestamp = t_location->timestamp;
    location.data = t_location->data;
    return location;
}

class ClientInterfaceAdapter : public ClientInterface {
public:
    ClientInterfaceAdapter()
    {
        Semaphore sema;
        m_thread.start([&]() {
            rclcpp::init(0, nullptr);
            m_node = rclcpp::Node::make_shared("mifsa_gnss_client");
            m_callbackGroup = m_node->create_callback_group(rclcpp::CallbackGroupType::Reentrant);
            auto qosConfig = rclcpp::QoS(rclcpp::KeepLast(10)).reliable();
            m_locationSub = m_node->create_subscription<mifsa_gnss_idl::msg::Location>("/mifsa/gnss/location", qosConfig, [this](mifsa_gnss_idl::msg::Location::UniquePtr t_location) {
                if (cbLocation) {
                    cbLocation(_getLocation(t_location));
                }
            });
            m_commandPub = m_node->create_publisher<mifsa_gnss_idl::msg::Command>("/mifsa/gnss/command", qosConfig);
            m_nmeaClient = m_node->create_client<mifsa_gnss_idl::srv::Nmea>("mifsa_gnss_server_nmea", ::rmw_qos_profile_default, m_callbackGroup);
            m_connected = connected();
            if (m_connected) {
                cbConnected(m_connected);
            }
            m_timer = m_node->create_wall_timer(std::chrono::milliseconds(500), [this]() {
                bool isConnected = connected();
                if (m_connected != isConnected) {
                    m_connected = isConnected;
                    cbConnected(m_connected);
                }
            });
            sema.reset();
            rclcpp::executors::MultiThreadedExecutor exec;
            exec.add_node(m_node);
            exec.spin();
        });
        sema.acquire();
    }
    ~ClientInterfaceAdapter()
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
    virtual std::string version() override
    {
        return MIFSA_GNSS_VERSION;
    }
    virtual bool connected() override
    {
        return m_nmeaClient->service_is_ready();
    }
    virtual std::string getNmea() override
    {
        auto ros_nmeaReq = std::make_shared<mifsa_gnss_idl::srv::Nmea::Request>();
        ros_nmeaReq->type = mifsa_gnss_idl::srv::Nmea::Request::QUERY_NMEA;
        auto result = m_nmeaClient->async_send_request(ros_nmeaReq);
        auto status = result.wait_for(std::chrono::milliseconds(_time_out));
        if (status == std::future_status::ready) {
            return result.get()->data;
        } else {
            LOG_WARNING("invoke failed");
            return "";
        }
    }
    virtual void startNavigation(const CbLocation& cb) override
    {
        cbLocation = cb;
        mifsa_gnss_idl::msg::Command ros_command;
        ros_command.type = mifsa_gnss_idl::msg::Command::START_NAVIGATION;
        m_commandPub->publish(ros_command);
    }
    virtual void stopNavigation() override
    {
        mifsa_gnss_idl::msg::Command ros_command;
        ros_command.type = mifsa_gnss_idl::msg::Command::STOP_NAVIGATION;
        m_commandPub->publish(ros_command);
    }

private:
    Thread m_thread;
    rclcpp::Node::SharedPtr m_node;
    rclcpp::CallbackGroup::SharedPtr m_callbackGroup;
    rclcpp::Subscription<mifsa_gnss_idl::msg::Location>::SharedPtr m_locationSub;
    rclcpp::Publisher<mifsa_gnss_idl::msg::Command>::SharedPtr m_commandPub;
    rclcpp::Client<mifsa_gnss_idl::srv::Nmea>::SharedPtr m_nmeaClient;
    rclcpp::TimerBase::SharedPtr m_timer;
    bool m_connected = false;
    //
    CbLocation cbLocation;
};

}

MIFSA_NAMESPACE_END

#endif // MIFSA_GNSS_CLIENT_INTERFACE_ROS_H
