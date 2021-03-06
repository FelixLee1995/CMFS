cmake_minimum_required(VERSION 3.14)


#设定vckpg 路径
#SET(CMAKE_TOOLCHAIN_FILE, C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake)


project(CMFS)


#设定CPP版本
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")


add_definitions(-g)

include(FetchContent)

FetchContent_Declare(googletest
        GIT_REPOSITORY   https://gitee.com/paul3rd/gtest.git
        GIT_TAG          master
        TLS_VERIFY       OFF
        QUIET            OFF)
set(gtest_force_shared_crt ON  CACHE INTERNAL "" FORCE)
set(INSTALL_GTEST          OFF CACHE INTERNAL "" FORCE)
FetchContent_MakeAvailable(googletest)
#find_package(GTest CONFIG REQUIRED)




#设置UNICODE字符集
add_definitions(-DUNICODE -D_UNICODE -Wall)

add_definitions(-DBOOST_ASIO_NO_DEPRECATED)


include_directories(./include)
include_directories(./dependencies/include)

link_directories(./dependencies/lib)
link_directories(lib)

#扫描src目录, 获取.c .cpp 源码文件列表
file(GLOB_RECURSE DIR_SRCS ./src/*.cpp  ./src/*.c)
set(TEST_CACHE_SRC ./test/test_cache.cpp)
set(TEST_MARKET_DATA_SRC ./test/test_marketdata.cpp)

set(TEST_MD_SRC ./test/test_md_sub.cpp ./src/apiWrapper/ctp/MyMarketApi.cpp ./src/apiWrapper/ctp/MyMarketSpi.cpp)
set(Qry_SRC ./test/InstrumentQryProcess.cpp ./src/apiWrapper/ctp/MyCtpApi.cpp ./src/apiWrapper/ctp/MyCtpSpi.cpp)


# 可执行文件输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/runtime)


#使用源码列表生成可执行文件
ADD_EXECUTABLE(cmfs ${DIR_SRCS})
ADD_EXECUTABLE(qry_ctp ${Qry_SRC})




target_link_libraries(cmfs gtest GtjaMdUserApi thostmduserapi_se thosttraderapi_se)
target_link_libraries(qry_ctp  thosttraderapi_se)

IF(UNIX)
  target_link_libraries(qry_ctp pthread )
  target_link_libraries(cmfs pthread)
ENDIF()




#设置msvc编译选项 -- 目标是否使用静态运行时 
IF(MSVC) 
  target_compile_options(cmfs PUBLIC /MT)
ENDIF()



