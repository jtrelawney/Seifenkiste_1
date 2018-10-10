cmake_minimum_required(VERSION 3.0)

project(RPI_tcp_test)

include_directories(src src/tcp_client ../common)
add_library(tcp_client SHARED src/tcp_client/tcp_client.cpp)
add_library(messages SHARED ../common/message_class.cpp)

#this works but it is annyoing having to switch dir to run it
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "..")

set(PPROJECT_SOURCES src/tcp_client/tcp_client.cpp src/tcp_client/tcp_client.h)
 
add_executable(RPI_tcp_test src/tcp_client_test.cpp)
target_link_libraries(RPI_tcp_test tcp_client)
target_link_libraries(RPI_tcp_test messages)








#this didn't work, the library always ended up in build
#install(TARGETS tcp_client DESTINATION ./lib)
