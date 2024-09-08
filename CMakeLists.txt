cmake_minimum_required(VERSION 3.22) # 最低CMAKE版本,3.22兼容ubuntu22到ubuntu24
# cmake构造时的--target后的参数
set(project_name main)
project(${project_name}) # 工程名
# C++语言标准20
set(CMAKE_CXX_STANDARD 20)
# 生成可执行文件,参与生成文件main.cpp 生成的可执行文件名为linux_network_prog
add_executable(${project_name} main.cpp) #生成main可执行程序
add_executable(server server.cpp) #生成server可执行程序
add_executable(client client.cpp) #生成client可执行程序
add_executable(server_multi server_multi.cpp) #生成server_multi可执行文件
add_executable(test test.cpp) #测试工程
