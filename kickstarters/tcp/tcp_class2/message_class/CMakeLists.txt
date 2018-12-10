cmake_minimum_required(VERSION 3.0)

project(buffer_class_test)

# compiler flags
SET(GCC_ADDITIONAL_FLAGS "-std=c++11 -Wall -pedantic -Wextra -Werror -pthread -g -Og")
#r

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_ADDITIONAL_FLAGS}")
message ("\nsetting additional compiler flags:")
message (${CMAKE_CXX_FLAGS} "\n")


# where the libs are compiled into
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
message ("\nsetting library output path:")
message (${CMAKE_LIBRARY_OUTPUT_DIRECTORY} "\n")


# setting directories
set(PROJECT_PATH "/home/ethan/projects/Seifenkiste_1")
set(DIR_KICKSTARTER ${PROJECT_PATH}/kickstarters)
set(DIR_PROJECT ${DIR_KICKSTARTER}/tcp/tcp_class/buffer_class)
set(DIR_LIBS ${PROJECT_PATH}/lib)
set(DIR_LOCAL_LIBS ${DIR_KICKSTARTER}/tcp_class)

message ("\nusing the following source directories for the main and libs:")
#message (${DIR_LIBS})
message (${DIR_PROJECT})
#message (${DIR_LOCAL_LIBS})



# include directories
include_directories( ${DIR_PROJECT} )
#include_directories( ${DIR_PROJECT} ${DIR_LIBS} ${DIR_LOCAL_LIBS} )
message ("\n\nheader dirs:")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

# set libraries to use for this project
#set(OPENCV_LIBS opencv_core opencv_highgui opencv_imgproc opencv_video opencv_videoio)
set(MY_LIBS buffer_class)
set(GENERAL_LIBS rt)
set(ALL_PROJECT_LIBS ${GENERAL_LIBS} ${MY_LIBS})
message ("\n\nsetting libraries as :\n" ${ALL_PROJECT_LIBS} "\n")


# define project targets, what will be compiled
#set(PROJECT_SOURCES  ${DIR_COMMON}/message_class.cpp ${DIR_PROJECT}/stream_rpi_camera.cpp )

#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/1_concurrent_access.cpp)
#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/2_concurrent_access_class_mutex.cpp)
#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/3_concurrent_access_queue.cpp)
set(PROJECT_SOURCES  ${DIR_PROJECT}/buffer_class_test.cpp)

message ("\n\nmain target to compile")
message ("${PROJECT_SOURCES}")

# add targets 
#add_library(common_stuff SHARED ${DIR_COMMON}/defs.cpp)
#add_library(message_class SHARED ${DIR_PROJECT}/message_class.cpp)
#add_library(tcp_class SHARED ${DIR_PROJECT}/tcp_class.cpp)
#add_library(message_queue_class SHARED ${DIR_PROJECT}/message_queue_class.cpp)
#add_library(message_queue_class SHARED ${DIR_COMMON}/message_queue_class.cpp)
#add_library(tcp_server SHARED ${DIR_PC_LIBS}/tcp_server.cpp)

add_library(buffer_class SHARED ${DIR_PROJECT}/buffer_class.cpp)

#add_executable(1_concurrent_access ${PROJECT_SOURCES} )
#add_executable(2_concurrent_access_class_mutex ${PROJECT_SOURCES} )
#add_executable(3_concurrent_access_queue ${PROJECT_SOURCES} )
add_executable(buffer_class_test ${PROJECT_SOURCES} )


#target_link_libraries (message_class common_stuff)
#target_link_libraries (message_queue_class common_stuff message_class)
#target_link_libraries (tcp_server common_stuff message_class message_queue_class)


#target_link_libraries (1_concurrent_access ${ALL_PROJECT_LIBS})
#target_link_libraries (3_concurrent_access_queue ${ALL_PROJECT_LIBS})
target_link_libraries (buffer_class_test ${ALL_PROJECT_LIBS})


