cmake_minimum_required(VERSION 3.0)

project(camera_tcp_test)

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++11")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

include_directories(tcp_client ../common)
add_library(COMMON_stuff SHARED ../common/defs.cpp)
add_library(tcp_client SHARED tcp_client/tcp_client.cpp)
add_library(messages SHARED ../common/message_class.cpp)

add_executable(camera_tcp_test dev_tests/camera_tcp_test.cpp)
target_link_libraries (camera_tcp_test messages COMMON_stuff tcp_client opencv_core opencv_highgui opencv_imgproc opencv_video opencv_videoio)

add_executable(RPI_tcp_test_message dev_tests/tcp_client_test_message.cpp)
target_link_libraries (RPI_tcp_test_message COMMON_stuff messages tcp_client)

add_executable(RPI_tcp_test_timer_messages dev_tests/tcp_client_test_timer_messages.cpp)
target_link_libraries (RPI_tcp_test_timer_messages COMMON_stuff messages tcp_client rt)