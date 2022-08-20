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

mifsa_module_begin(client c)

file(GLOB_RECURSE
    PROJECT_SOURCES
    ${PROJECT_SOURCE_DIR}/include/*.h
    ${PROJECT_SOURCE_DIR}/src/*.*
    )
add_library(
    ${PROJECT_NAME}
    ${PROJECT_SOURCES}
    )
target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    mifsa_${MIFSA_MODULE_TYPE}
)

mifsa_module_end()
