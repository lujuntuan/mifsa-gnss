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
#include "interface_fdbus.hpp"
#include "interface_ros.hpp"
#include "interface_vsomeip.hpp"

MIFSA_NAMESPACE_BEGIN

namespace Gnss {

Client::Client(int argc, char** argv)
    : ClientProxy(argc, argv, "mifsa_gnss_server")
{
    setInstance(this);
    //
    loadInterface<InterfaceImplementation>();
}

Client::~Client()
{
    setInstance(nullptr);
}

}

MIFSA_NAMESPACE_END
