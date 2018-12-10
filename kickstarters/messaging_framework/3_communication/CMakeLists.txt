cmake_minimum_required(VERSION 3.0)

project(process_com)

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
set(DIR_PROJECT ${DIR_KICKSTARTER}/messaging_framework/3_communication)

message ("\nusing the following source directories for the main and libs:")
message (${DIR_KICKSTARTER})
message (${DIR_PROJECT})

# include directories
include_directories( ${DIR_PROJECT} ${DIR_KICKSTARTER} )
message ("\n\nheader dirs:")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

# set libraries to use for this project
#set(OPENCV_LIBS opencv_core opencv_highgui opencv_imgproc opencv_video opencv_videoio)
set(MY_LIBS process_com)
set(GENERAL_LIBS rt)
set(ALL_PROJECT_LIBS ${GENERAL_LIBS} ${MY_LIBS})
message ("\n\nsetting libraries as :\n" ${ALL_PROJECT_LIBS} "\n")


# define project targets, what will be compiled
#set(PROJECT_SOURCES  ${DIR_COMMON}/message_class.cpp ${DIR_PROJECT}/stream_rpi_camera.cpp )

#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/1_concurrent_access.cpp)
#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/2_concurrent_access_class_mutex.cpp)
#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/3_concurrent_access_queue.cpp)
set(PROJECT_SOURCES  ${DIR_PROJECT}/process_test.cpp)

message ("\n\nmain target to compile")
message ("${PROJECT_SOURCES}")

# add targets 
add_library(process_com SHARED ${DIR_PROJECT}/process_communication_class.cpp)
add_executable(process_test ${PROJECT_SOURCES} )
target_link_libraries (process_test ${ALL_PROJECT_LIBS})

