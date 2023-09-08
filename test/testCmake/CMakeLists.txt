cmake_minimum_required(VERSION 3.10)
project(main)

#设置可执行文件的存放位置
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#配置头文件搜索路径
#include_directories()
#配置库文件搜索路径
#link_directories()

#设置需要编译的源文件列表
set(SRC_LIST ./muduo_server.cpp)

#生成可执行文件
add_executable(muduo_server.out ${SRC_LIST})

#需要连接的头文件
target_link_libraries(muduo_server.out muduo_net muduo_base pthread)