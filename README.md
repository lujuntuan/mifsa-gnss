# MIFSA-GNSS

The GPS module in [mifsa](https://github.com/lujuntuan/mifsa).

Detailed documentation is found in [introduction](doc/introduction-zh.md)(中文).

Note: This is a simple engineering example, the specific business is not implemented.

## Features

TODO

## Requirements:

- [mifsa-base](https://github.com/lujuntuan/mifsa-base)

One of the following rpc communication libraries is required: 

- [fdbus](https://gitee.com/jeremyczhen/fdbus) [protobuf](https://github.com/protocolbuffers/protobuf)

- [commonapi-core-runtime](https://github.com/COVESA/capicxx-core-runtime) [commonapi-someip-runtime](https://github.com/COVESA/capicxx-someip-runtime) [capicxx-core-tools](https://github.com/COVESA/capicxx-core-tools) [capicxx-someip-tools](https://github.com/COVESA/capicxx-someip-tools)

- [rclcpp(ros2)](https://github.com/ros2/rclcpp)

Note: Also supports custom rpc.

## How to build:

```cmake
cmake -B build
cmake --build build --target install
```

Optional:

- -DMIFSA_BUILD_EXAMPLES: 

  whether to compile the examples, default is on.

- -DMIFSA_BUILD_TESTS :

  whether to compile the tests, default is on.

- -DMIFSA_IDL_TYPE: 

  Select the soa communication component (idl), support auto/ros/vsomeip/fdbus, default is auto.

Examples:
```shell
cmake -B build \
	-DCMAKE_INSTALL_PREFIX=build/install \
	-DMIFSA_BUILD_EXAMPLES=ON \
	-DMIFSA_BUILD_TESTS=OFF \
	-DMIFSA_IDL_TYPE=fdbus
cmake --build build --target install -j8
```

```shell
Note:
For vsomeip support, capicxx-core-tools Need to link to the bin directory.
Example:
ln -sf [capicxx-core-tools dir]/commonapi-core-generator-linux-x86_64 /usr/local/bin/capicxx-core-gen
ln -sf [capicxx-core-tools dir]/commonapi-someip-generator-linux-x86_64 /usr/local/bin/capicxx-someip-gen

For ros2 support, you should set following environment
export AMENT_PREFIX_PATH=[ros2 install dir] #example: opt/ros2_install
export CMAKE_PREFIX_PATH=[ros2 install dir] #example: opt/ros2_install
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH;[ros2 lib install dir] #example: opt/ros2_install/lib
export PYTHONPATH=[ros2 python install path] #example: opt/ros2_install/lib/python3.10/site-packages
```

## How to use:

In CMakeLists.txt:

```cmake
...
find_package(mifsa_gnss REQUIRED)
target_link_libraries(
    [TARGET]
    mifsa_gnss
    )
...
```

In cpp code:

```c++
#include <mifsa/gnss/client.h>

using namespace Mifsa;

int main(int argc, char* argv[])
{
    Gnss::Client client(argc, argv);
    client.detectConnect([&](bool connect) {
        if (connect) {
            client.interface()->startNavigation([&](const Gnss::Location& location) {
                LOG_DEBUG("***** print Gnss::Location");
                const std::string& nmea = client.interface()->getNmea();
                LOG_PROPERTY("nmea", nmea);
                LOG_PROPERTY("size", location.size);
                LOG_PROPERTY("flags", location.flags);
                LOG_PROPERTY("latitude", location.latitude);
                LOG_PROPERTY("longitude", location.longitude);
                LOG_PROPERTY("altitude", location.altitude);
                LOG_PROPERTY("speed", location.speed);
                LOG_PROPERTY("bearing", location.bearing);
                LOG_PROPERTY("accuracy", location.accuracy);
                LOG_PROPERTY("timestamp", location.timestamp);
                LOG_PROPERTY("data-size", location.data.size());
                LOG_DEBUG("*****\n");
            });
        }
    });
    int reval = client.exec();
    client.interface()->stopNavigation();
    return reval;
}
```

In c code:

```c
#include <inttypes.h>
#include <mifsa/gnss/client_c.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef WIN32
#include <Windows.h>
#define SSLEEP(t) Sleep(t * 1000)
#else
#include <unistd.h>
#define SSLEEP(t) sleep(t)
#endif

static void on_connect_changed(bool connected)
{
    if (connected) {
        printf("server connected\n");
    } else {
        printf("server disconnected\n");
    }
}

static void on_printf_location(const mifsa_gnss_location_t* location)
{
    printf("*****print gnss location\n");
    char nmea[150] = { 0 };
    char* nmea_addr = nmea;
    int nmea_size = 0;
    mifsa_gnss_get_nmea_data(&nmea_addr, &nmea_size);
    printf("%s: %s\n", "nmea-data", nmea);
    printf("%s: %d\n", "size", location->size);
    printf("%s: %d\n", "flags", location->flags);
    printf("%s: %lf\n", "latitude", location->latitude);
    printf("%s: %lf\n", "longitude", location->longitude);
    printf("%s: %lf\n", "altitude", location->altitude);
    printf("%s: %f\n", "speed", location->speed);
    printf("%s: %f\n", "bearing", location->bearing);
    printf("%s: %f\n", "accuracy", location->accuracy);
    printf("%s: %" PRId64 "\n", "timestamp", location->timestamp);
    printf("%s: %d\n", "data-size", location->data_len);
    printf("*****\n\n");
    fflush(stdout);
}

int main(int argc, char* argv[])
{
    mifsa_gnss_cb_tables_t cb_tables = {
        on_connect_changed,
        on_printf_location,
    };
    mifsa_gnss_init(cb_tables);
    mifsa_gnss_waitfor_server(-1);
    mifsa_gnss_start_navigation(on_printf_location);
    SSLEEP(20);
    mifsa_gnss_stop_navigation();
    mifsa_gnss_deinit();
    return 0;
}
```

Note: For more usage, please see the code in the [examples](examples) folder.

## Copyright:

Juntuan.Lu, 2020-2030, All rights reserved.
