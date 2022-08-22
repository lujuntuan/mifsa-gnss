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
#include "mifsa/gnss/platform_interface.h"
#include "mifsa/gnss/server_interface.h"
#include <mifsa/base/singleton.h>
#include <mifsa/module/server.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
class Server : public ServerProxy<ServerInterface>, public PlatformProxy<PlatformInterface>, public SingletonProxy<Server> {

public:
    Server(int argc, char** argv);
    ~Server();

    void reportGnss();
    void parserNmea(const std::string& nmea, Location& location);

protected:
    virtual void eventChanged(const std::shared_ptr<Event>& event) override;

private:
    friend class ServertInterfaceAdapter;
    Location m_location;
};
}

MIFSA_NAMESPACE_END

#define mifsa_gnss_server Mifsa::Gnss::Server::getInstance()

#endif // MIFSA_GNSS_SERVER_H
