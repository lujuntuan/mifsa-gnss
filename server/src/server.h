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

#ifndef MIFSA_GNSS_SERVER_H
#define MIFSA_GNSS_SERVER_H

#include "mifsa/gnss/config.h"
#include "mifsa/gnss/platform.h"
#include "mifsa/gnss/provider.h"
#include <mifsa/base/singleton.h>
#include <mifsa/module/server.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Gnss {

class Server : public ServerProxy<Provider>, public PlatformProxy<Platform>, public SingletonProxy<Server> {

public:
    Server(int argc, char** argv);
    ~Server();

    void reportGnss();
    void parserNmea(const std::string& nmea, Location& location);

protected:
    virtual void eventChanged(const std::shared_ptr<Event>& event) override;

private:
    Location m_location;
};

}

MIFSA_NAMESPACE_END

#define mifsa_gnss_server Mifsa::Gnss::Server::getInstance()
#define mifsa_gnss_provider mifsa_gnss_server->provider()
#define mifsa_gnss_platform mifsa_gnss_server->platform()

#endif // MIFSA_GNSS_SERVER_H
