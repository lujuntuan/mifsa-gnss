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

mifsa_module_begin(platform)

file(GLOB_RECURSE
    MIFSA_PROJECT_SOURCES
    ${PROJECT_SOURCE_DIR}/*.c
    ${PROJECT_SOURCE_DIR}/*.cpp
    )

add_library(
    ${PROJECT_NAME}
    ${MIFSA_PROJECT_SOURCES}
    )

mifsa_module_end()
