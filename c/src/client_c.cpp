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

#include "mifsa/gnss/client_c.h"
#include <memory.h>
#include <mifsa/gnss/client.h>

using namespace Mifsa::Gnss;

std::shared_ptr<Client> _gnss_client;
mifsa_gnss_cb_tables_t _cb_tables;

int mifsa_gnss_init(mifsa_gnss_cb_tables_t cbs)
{
    if (_gnss_client) {
        return 1;
    }
    _cb_tables = cbs;
    _gnss_client = std::make_shared<Client>(0, nullptr);
#ifdef WIN32
    _gnss_client->setInstanceForce(_gnss_client.get());
#endif
    _gnss_client->detectConnect([](bool connected) {
        if (_cb_tables.connected_cb) {
            _cb_tables.connected_cb(connected);
        }
    });
    _gnss_client->asyncExec();
    return 0;
}

int mifsa_gnss_deinit()
{
    if (!_gnss_client) {
        return 1;
    }
    _gnss_client->exit();
    _gnss_client.reset();
    return 0;
}

int mifsa_gnss_waitfor_server(int timeout_ms)
{
    if (!_gnss_client) {
        return 1;
    }
    bool ok = _gnss_client->waitForConnected(timeout_ms);
    if (ok) {
        return 0;
    } else {
        return 3;
    }
}

int mifsa_gnss_get_nmea_data(char** nmea, int* size)
{
    if (!nmea || !size) {
        return 2;
    }
    std::string data = _gnss_client->interface()->getNmea().c_str();
    memcpy(*nmea, data.c_str(), data.size());
    *size = (int)data.size();
    return 0;
}

int mifsa_gnss_start_navigation()
{
    if (!_gnss_client) {
        return 1;
    }
    _gnss_client->interface()->startNavigation([&](const Location& location) {
        if (_cb_tables.location_cb) {
            mifsa_gnss_location_t c_location;
            c_location.size = location.size;
            c_location.flags = location.flags;
            c_location.latitude = location.latitude;
            c_location.longitude = location.longitude;
            c_location.altitude = location.altitude;
            c_location.speed = location.speed;
            c_location.bearing = location.bearing;
            c_location.accuracy = location.accuracy;
            c_location.timestamp = location.timestamp;
            c_location.data = (char*)location.data.c_str();
            c_location.data_len = location.data.size();
            _cb_tables.location_cb(&c_location);
        }
    });
    return 0;
}

int mifsa_gnss_stop_navigation()
{
    if (!_gnss_client) {
        return 1;
    }
    _gnss_client->interface()->stopNavigation();
    return 0;
}
