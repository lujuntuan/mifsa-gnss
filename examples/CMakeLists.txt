#*********************************************************************************
#  *Copyright(C): Juntuan.Lu, 2020-2030, All rights reserved.
#  *Author:  Juntuan.Lu
#  *Version: 1.0
#  *Date:  2022/04/01
#  *Email: 931852884@qq.com
#  *Description:
#  *Others:
#  *Function List:
#  *History:
#**********************************************************************************

project(examples)

add_executable(
    mifsa_gnss_sample
    sample.cpp
    )
add_executable(
    mifsa_gnss_sample_c
    sample.c
    )
target_link_libraries(
    mifsa_gnss_sample
    PRIVATE
    mifsa_base
    mifsa_gnss
)
target_link_libraries(
    mifsa_gnss_sample_c
    PRIVATE
    mifsa_base
    mifsa_gnss
    mifsa_gnss_c
)
install(
    TARGETS
    mifsa_gnss_sample
    mifsa_gnss_sample_c
    RUNTIME
    DESTINATION
    ${CMAKE_INSTALL_BINDIR}
    ARCHIVE
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
    LIBRARY
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
    )
