cmake_minimum_required(VERSION 3.7.2)

project(tcp_test_run)

include_directories(/home/pi/projects/Seifenkiste_1/RPI_cockpit_1)
add_library(tcp_client SHARED tcp_client.cpp)

#this didn't work, the library always ended up in build
#install(TARGETS tcp_client DESTINATION ./lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "..")

add_executable(tcp_test tcp_client_test.cpp)
target_link_libraries(tcp_test tcp_client)
