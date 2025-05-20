# 设置CMake的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(ZSTL)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

# 查找Google Test库
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# 添加可执行文件
add_executable(zstl_test test/test.cc)

# 链接Google Test库和pthread库
target_link_libraries(zstl_test ${GTEST_LIBRARIES} pthread)

# 添加测试
enable_testing()
add_test(NAME ZSTLTests COMMAND zstl_test)