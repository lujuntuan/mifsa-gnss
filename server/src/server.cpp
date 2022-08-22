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
#include "adapter/adapter.h"
#include "parser/minmea.h"
#include <sstream>

#define m_hpr m_serverHelper

int main(int argc, char* argv[])
{
    Mifsa::Gnss::Server server(argc, argv);
    return server.exec();
}

MIFSA_NAMESPACE_BEGIN

namespace Gnss {
struct ServerHelper {
    Location location;
    const Application::Arg argVersion { "v", "version", " module version" };
};

Server::Server(int argc, char** argv)
    : ServerProxy(argc, argv, "gnss")
{
    setInstance(this);
    MIFSA_HELPER_CREATE(m_hpr);
    //
    parserArgs({ m_hpr->argVersion });
    if (getArgValue(m_hpr->argVersion).toBool()) {
        LOG_DEBUG(MIFSA_GNSS_VERSION);
        std::exit(0);
        return;
    }
    //
    loadPlatform(module());
    loadInterface<ServertInterfaceAdapter>();
    std::weak_ptr<Timer> timer = createTimer(1000, true, [this]() {
        reportGnss();
    });
    interface()->setCbNmea([this](std::string& nmea) {
        nmea = platform()->getNmea();
    });
    interface()->setCbStartNavigation([this, timer]() {
        reportGnss();
        timer.lock()->start();
    });
    interface()->setCbStopNavigation([timer]() {
        timer.lock()->stop();
    });
}

Server::~Server()
{
    MIFSA_HELPER_DESTROY(m_hpr);
    setInstance(nullptr);
}

void Server::reportGnss()
{
    const std::string& nmea = platform()->getNmea();
    parserNmea(nmea, m_hpr->location);
    std::string data;
    data.resize(1024);
    data.append("data");
    m_hpr->location.data = data;
    interface()->reportGnss(m_hpr->location);
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
}

}
MIFSA_NAMESPACE_END
