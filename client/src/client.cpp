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

#include "mifsa/gnss/client.h"
#include "adapter/client_interface_fdbus.hpp"
#include "adapter/client_interface_ros.hpp"
#include "adapter/client_interface_vsomeip.hpp"

MIFSA_NAMESPACE_BEGIN

namespace Gnss {

Client::Client(int argc, char** argv)
    : ClientProxy(argc, argv, "mifsa_gnss_server")
{
    setInstance(this);
    //
    loadInterface<ClientInterfaceAdapter>();
}

Client::~Client()
{
    setInstance(nullptr);
}

std::string Client::getNmea()
{
    return interface()->getNmea();
}

void Client::startNavigation(const ClientInterface::CbLocation& cb)
{
    interface()->startNavigation(cb);
}

void Client::stopNavigation()
{
    interface()->stopNavigation();
}

}

MIFSA_NAMESPACE_END
