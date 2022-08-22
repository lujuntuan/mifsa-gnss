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

if (TARGET mifsa_${MIFSA_MODULE_TYPE})
    add_subdirectory(sample)
endif()
if (TARGET mifsa_${MIFSA_MODULE_TYPE}_c)
    add_subdirectory(sample_c)
endif()
