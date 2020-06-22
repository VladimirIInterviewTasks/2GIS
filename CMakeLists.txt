cmake_minimum_required(VERSION 3.0.0)
project(2GIS VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
set(GTEST_ROOT "C:\\Program Files (x86)\\googletest-distribution\\" CACHE PATH "Path to googletest")
find_package(GTest REQUIRED)

add_executable(2GIS main.cpp node.cpp)

add_executable(GTEST test.cpp node.cpp) 
include_directories(${GTEST_INCLUDE_DIRS})

#set_property(TARGET GTEST PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
#not working using manual 


set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CFLAGS} /DEBUG:FULL /ZI")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CFLAGS} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CFLAGS} /MTd /Od)")

target_link_libraries(GTEST ${GTEST_BOTH_LIBRARIES})
ADD_TEST(NAME GTEST COMMAND GTEST)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

