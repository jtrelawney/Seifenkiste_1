cmake_minimum_required(VERSION 3.0)

project(test_streamer)

set(PROJECT_PATH "~/home/ethan/projects/Seifenkiste_1")
set(DIR_COMMON ${PROJECT_PATH}/common)
set(DIR_PC ${PROJECT_PATH}/PC_cockpit_1)
set(DIR_DEV_TEST ${DIR_PC}/dev_tests)
set(DIR_MAIN ${DIR_PC}/src)

message ("\n using the following source and lib directories :\n")
message (${DIR_COMMON} "\n")
message (${DIR_PC} "\n")
message (${DIR_DEV_TEST} "\n")
message (${DIR_MAIN} "\n")



SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++11 -Wall -Werror")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


set(PROJECT_SOURCES stream_rpi_camera/stream_rpi_camera.cpp)
add_executable(streamer PRIVATE ${PROJECT_SOURCES} )


# how many dires above does the project start

target_include_directories( streamer ${DIR_COMMON} )

add_library(tcp_server SHARED ../tcp_server/tcp_server.cpp)
add_library(message_class SHARED ../../common/message_class.cpp)
add_library(common_stuff SHARED ../../common/defs.cpp)


set(OPENCV_LIBS opencv_core opencv_highgui opencv_imgproc opencv_video opencv_videoio)
set(MY_LIBS tcp_server message_class common_stuff)
set(PROJECT_LIBS ${OPENCV_LIBS} ${MY_LIBS})
message ("\n\nsetting libraries as :\n" ${PROJECT_LIBS} "\n")


#target_link_libraries(streamer tcp_server)
#target_link_libraries(streamer message_class)
#target_link_libraries(streamer common_stuff)
target_link_libraries (streamer ${PROJECT_LIBS})
