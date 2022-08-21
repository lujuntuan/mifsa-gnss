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

#ifndef MIFSA_GNSS_CLIENT_H
#define MIFSA_GNSS_CLIENT_H

#include "mifsa/gnss/config.h"
#include "mifsa/gnss/interface.h"
#include <mifsa/base/singleton.h>
#include <mifsa/module/client.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
class MIFSA_EXPORT Client : public ClientProxy<Interface>, public SingletonProxy<Client> {
public:
    Client(int argc, char** argv);
    ~Client();

private:
    friend class InterfaceImplementation;
};
}

MIFSA_NAMESPACE_END

#define mifsa_gnss_client Mifsa::Gnss::Client::getInstance()
#define mifsa_gnss_interface mifsa_gnss_client->interface()

#endif // MIFSA_GNSS_CLIENT_H
