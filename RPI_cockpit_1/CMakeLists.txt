cmake_minimum_required(VERSION 3.0)

project(RPI_tcp_test)

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++11")
SET(GCC_COVERAGE_LINK_FLAGS    "-pthread")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

include_directories(src src/tcp_client ../common)
add_library(tcp_client SHARED src/tcp_client/tcp_client.cpp)
add_library(messages SHARED ../common/message_class.cpp)
add_library(COMMON_stuff ../common/defs.cpp)

#this works but it is annyoing having to switch dir to run it
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "..")

set(PPROJECT_SOURCES src/tcp_client/tcp_client.cpp src/tcp_client/tcp_client.h)
 
add_executable(RPI_tcp_test_message src/tcp_client_test_message.cpp)
target_link_libraries(RPI_tcp_test_message tcp_client)
target_link_libraries(RPI_tcp_test_message messages)
target_link_libraries(RPI_tcp_test_message COMMON_stuff)


add_executable(RPI_tcp_test_timer_messages src/tcp_client_test_timer_messages.cpp)
target_link_libraries(RPI_tcp_test_timer_messages tcp_client)
target_link_libraries(RPI_tcp_test_timer_messages messages)
target_link_libraries(RPI_tcp_test_timer_messages COMMON_stuff)

message(STATUS "building complex_timer and need to use the rt -library")

find_library(
    LIB_I_NEED rt
    HINTS "path_to_library"
)

message(STATUS "Found 'external_lib' at: ${LIB_I_NEED}")
#add_executable(complex_timer_test timers/complex_timer.cpp)
target_link_libraries (RPI_tcp_test_timer_messages ${LIB_I_NEED})




#this didn't work, the library always ended up in build
#install(TARGETS tcp_client DESTINATION ./lib)
