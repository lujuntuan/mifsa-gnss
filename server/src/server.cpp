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

#include "server.h"
#include "adpter/provider_fdbus.hpp"
#include "adpter/provider_ros.hpp"
#include "adpter/provider_vsomeip.hpp"
#include "parser/minmea.h"
#include <sstream>

int main(int argc, char* argv[])
{
    Mifsa::Gnss::Server server(argc, argv);
    return server.exec();
}

MIFSA_NAMESPACE_BEGIN

class EventDemo : public Event {
public:
    enum Type {
        DEMO_1,
        DEMO_2,
        DEMO_3
    };
    explicit EventDemo(Type type, const Variant& data = Variant())
        : Event(0, type)
        , m_data(data)
    {
    }
    inline const Variant& data()
    {
        return m_data;
    }
    inline void setData(const Variant& data)
    {
        m_data = data;
    }

private:
    Variant m_data;
};

namespace Gnss {

const Application::Arg _arg_version("v", "version", " module version");
Server::Server(int argc, char** argv)
    : ServerProxy(argc, argv, "gnss")
{
    setInstance(this);
    //
    parserArgs({ _arg_version });
    if (getArgValue(_arg_version).toBool()) {
        LOG_DEBUG(MIFSA_GNSS_VERSION);
        std::exit(0);
    }
    //
    loadPlatform(module());
    loadProvider<ProviderImplementation>();
    std::weak_ptr<Timer> timer = createTimer(100, true, [this]() {
        reportGnss();
        // test
        // VariantMap data1 = { { "name", "tuan" }, { "arg", "18" } };
        // postEvent(std::make_shared<EventDemo>(EventDemo::DEMO_1, data1));
    });
    provider()->setCbNmea([this](std::string& nmea) {
        nmea = platform()->getNmea();
    });
    provider()->setCbStartNavigation([this, timer]() {
        reportGnss();
        timer.lock()->start();
    });
    provider()->setCbStopNavigation([timer]() {
        timer.lock()->stop();
    });
}

Server::~Server()
{
    setInstance(nullptr);
}

void Server::reportGnss()
{
    const std::string& nmea = platform()->getNmea();
    parserNmea(nmea, m_location);
    std::string data;
    data.resize(1024);
    data.append("data");
    m_location.data = data;
    provider()->reportGnss(m_location);
}

void Server::parserNmea(const std::string& nmea, Location& location)
{
    std::istringstream stream(nmea);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty()) {
            continue;
        }
        enum minmea_sentence_id sentence_id = minmea_sentence_id(line.c_str(), false);
        if (sentence_id == MINMEA_INVALID) {
            LOG_DEBUG("$xxxxx sentence is not valid, nmea data=", line);
            continue;
        }
        switch (sentence_id) {
        case MINMEA_SENTENCE_GBS: {
            struct minmea_sentence_gbs frame;
            if (minmea_parse_gbs(&frame, line.c_str())) {
            } else {
                LOG_DEBUG("$xxGBS sentence is not parsed, nmea data=", line);
            }
        } break;
        case MINMEA_SENTENCE_GGA: {
            struct minmea_sentence_gga frame;
            if (minmea_parse_gga(&frame, line.c_str())) {
                location.latitude = minmea_todouble(&frame.latitude);
                location.longitude = minmea_todouble(&frame.longitude);
                location.longitude = minmea_todouble(&frame.altitude);
            } else {
                LOG_DEBUG("$xxGGA sentence is not parsed, nmea data=", line);
            }
        } break;
        case MINMEA_SENTENCE_GLL: {
            struct minmea_sentence_gll frame;
            if (minmea_parse_gll(&frame, line.c_str())) {
            } else {
                LOG_DEBUG("$xxGLL sentence is not parsed, nmea data=", line);
            }
        } break;
        case MINMEA_SENTENCE_GSA: {
            struct minmea_sentence_gsa frame;
            if (minmea_parse_gsa(&frame, line.c_str())) {
                location.timestamp = 100;
            } else {
                LOG_DEBUG("$xxGSA sentence is not parsed, nmea data=", line);
            }
        } break;
        case MINMEA_SENTENCE_GST: {
            struct minmea_sentence_gst frame;
            if (minmea_parse_gst(&frame, line.c_str())) {
                location.timestamp = 200;
            } else {
                LOG_DEBUG("$xxGST sentence is not parsed, nmea data=", line);
            }
        } break;
        case MINMEA_SENTENCE_GSV: {
            struct minmea_sentence_gsv frame;
            if (minmea_parse_gsv(&frame, line.c_str())) {
            } else {
                LOG_DEBUG("$xxGSV sentence is not parsed, nmea data=", line);
            }
        } break;
        case MINMEA_SENTENCE_RMC: {
            struct minmea_sentence_rmc frame;
            if (minmea_parse_rmc(&frame, line.c_str())) {
                timespec timestamp;
                minmea_gettime(&timestamp, &frame.date, &frame.time);
                location.timestamp = timestamp.tv_nsec * 1000;
                location.speed = minmea_tofloat(&frame.speed);
                location.latitude = minmea_todouble(&frame.latitude);
                location.longitude = minmea_todouble(&frame.longitude);
                // location.course=minmea_todouble(&frame.course);
            } else {
                LOG_DEBUG("$xxRMC sentence is not parsed, nmea data=", line);
            }
        } break;
        case MINMEA_SENTENCE_VTG: {
            struct minmea_sentence_vtg frame;
            if (minmea_parse_vtg(&frame, line.c_str())) {
            } else {
                LOG_DEBUG("$xxVTG sentence is not parsed, nmea data=", line);
            }
        } break;
        case MINMEA_SENTENCE_ZDA: {
            struct minmea_sentence_zda frame;
            if (minmea_parse_zda(&frame, line.c_str())) {
            } else {
                LOG_DEBUG("$xxZDA sentence is not parsed, nmea data=", line);
            }
        } break;
        case MINMEA_SENTENCE_ACCURACY: {
            struct minmea_sentence_accuracy frame;
            if (minmea_parse_accuracy(&frame, line.c_str())) {
                location.accuracy = minmea_tofloat(&frame.h_accuracy);
                //                location.vertical_accuracy=minmea_todouble(&frame.v_accuracy);
                //                location.speed_accuracy=minmea_todouble(&frame.s_accuracy);
                //                location.bearing_accuracy=minmea_todouble(&frame.b_accuracy);
            } else {
                LOG_DEBUG("$xxACCURACY sentence is not parsed, nmea data=", line);
            }
        } break;
        case MINMEA_SENTENCE_PMTKCHL: {
            struct minmea_sentence_pmtkchl frame;
            if (minmea_parse_pmtkchl(&frame, line.c_str())) {
            } else {
                LOG_DEBUG("$PMTKCHL sentence is not parsed, nmea data=", line);
            }
        } break;
        case MINMEA_SENTENCE_PMTKVNED: {
            struct minmea_sentence_pmtkvned frame;
            if (minmea_parse_pmtkvned(&frame, line.c_str())) {
                location.accuracy = minmea_tofloat(&frame.north_accuracy);
            } else {
                LOG_DEBUG("$PMTKVNED sentence is not parsed, nmea data=", line);
            }
        } break;
        case MINMEA_SENTENCE_PMTKGRP: {
            struct minmea_sentence_pmtkgrp frame;
            if (minmea_parse_pmtkgrp(&frame, line.c_str())) {

            } else {
                LOG_DEBUG("$PMTKGRP sentence is not parsed, nmea data=", line);
            }
        } break;
        default: {
            LOG_DEBUG("$xxxxx sentence is not parsed");
        } break;
        }
    }
    // end parser
}

void Server::eventChanged(const std::shared_ptr<Event>& event)
{
    if (!event || event->isAccepted()) {
        return;
    }
    switch (event->type()) {
    case EventDemo::DEMO_1: {
        auto demoEvent = std::dynamic_pointer_cast<EventDemo>(event);
        if (demoEvent) {
            // LOG_DEBUG(demoEvent->data());
        }
    } break;
    case EventDemo::DEMO_2: {
    } break;
    case EventDemo::DEMO_3: {
    } break;
    default:
        break;
    }
}

}
MIFSA_NAMESPACE_END
