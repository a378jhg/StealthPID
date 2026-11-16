cmake_minimum_required(VERSION 3.10)
project(zygisk_module)

# 添加你的 C++ 源文件
add_library(${CMAKE_PROJECT_NAME} SHARED example.cpp)

# 设置输出名字，必须叫这个，最后打包时要用
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES PREFIX "" OUTPUT_NAME "super_hide_fix")

# 编译选项
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -fno-exceptions)

# 链接 Android 日志库 (这里修正了 PRIVATE 的位置)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE log)
