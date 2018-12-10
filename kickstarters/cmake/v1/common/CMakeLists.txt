cmake_minimum_required(VERSION 3.0)

# from https://stackoverflow.com/questions/18968979/how-to-get-colorized-output-with-cmake
string(ASCII 27 Esc)
set(BoldRed     "${Esc}[1;31m")
set(Yellow      "${Esc}[33m")
set(Green       "${Esc}[32m")
set(BoldGreen   "${Esc}[1;32m")
set(ColourBold  "${Esc}[1m")
set(ColourReset "${Esc}[m")
#message("\n\ncmake script for cmake test main")
#message("${Green}\n\ncmake script for cmake test main${ColourReset}")
message("${BoldGreen}\n\ncmake script for cmake test common${ColourReset}")

#[[ this doesn't work when called locally because cmake doesn't know the variable
if(${MY_SCOPE} STREQUAL BUILD_ALL_LIBS)
    message("\nThe build scope is ALL LIBS = ${MY_SCOPE}")
else()
    message("\nUSE LOCAL SETTINGS = ${MY_SCOPE}")
    set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -pedantic -Wextra -Werror -pthread")
    # add flags to only debug -g -Og  (valgrind, profiling etc)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Og -pg")
    # add flags to only rekease -O optimizer level 3 and no debugsymbols etc)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

message("\nThe compiler flags are set to")
message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")
message("CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}")
]]

if(USE_CALLER_CONFIG)
    message("\ncaller config = ${USE_CALLER_CONFIG}")
else()
    message("\nuse own config  callerconfig = ${USE_CALLER_CONFIG}")
    set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -pedantic -Wextra -Werror -pthread")
    # add flags to only debug -g -Og  (valgrind, profiling etc)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Og -pg")
    # add flags to only rekease -O optimizer level 3 and no debugsymbols etc)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif(USE_CALLER_CONFIG)


message("\nThe compiler flags are set to")
message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")
message("CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}")

#home dir is set to caller or the local one
message("\ncurrent home dir is ${CMAKE_HOME_DIRECTORY}")
#source dir will always to local when the subdir is called
message("current source dir is ${CMAKE_CURRENT_SOURCE_DIR}")
#source dir is top of root when several layers are called or the local one when invoked directly
message("source dir is ${CMAKE_SOURCE_DIR}")


# here the output dirs are not set -> everything is built into the current dir
message("library output dir is ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message("runtime output dir is ${RUNTIME_OUTPUT_DIRECTORY}")

project(common)

# setting directories
#set(PROJECT_PATH "/home/ethan/projects/Seifenkiste_1")
#set(DIR_COMMON_LIBS ${PROJECT_PATH}/lib/common)

#message ("\nusing the following source directories for the libs:")
#message (${DIR_COMMON_LIBS})

# include directories
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
#message ("\n\nheader dirs:")
#get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
#foreach(dir ${dirs})
#  message(STATUS "dir='${dir}'")
#endforeach()


# where the libs are compiled into
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_PATH}/lib/lib_compiled)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#message ("\nsetting library output path:")
#message (${CMAKE_LIBRARY_OUTPUT_DIRECTORY} "\n")


# set libraries to use for this project
#set(OPENCV_LIBS opencv_core opencv_highgui opencv_imgproc opencv_video opencv_videoio)
set(MY_LIBS hello_lib)
set(GENERAL_LIBS rt)
set(OPENCV_LIBS opencv_core opencv_highgui opencv_imgproc opencv_video opencv_videoio)
set(ALL_PROJECT_LIBS ${GENERAL_LIBS} ${MY_LIBS} ${OPENCV_LIBS} )
message ("\n\nsetting libraries as :\n" ${ALL_PROJECT_LIBS} "\n")


# define project targets, what will be compiled
#set(PROJECT_SOURCES  ${DIR_COMMON}/message_class.cpp ${DIR_PROJECT}/stream_rpi_camera.cpp )

#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/1_concurrent_access.cpp)
#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/2_concurrent_access_class_mutex.cpp)
#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/3_concurrent_access_queue.cpp)
set(PROJECT_SOURCES test_hello_lib.cpp)

message ("\n\nunit test target to compile")
message ("${PROJECT_SOURCES}")

# add targets 
add_library(hello_lib SHARED hello_lib.cpp)
#add_library(common_communication SHARED ${DIR_COMMON_LIBS}/common_communication.cpp)
#dd_library(sensor_camera_class SHARED ${DIR_SENSOR_LIBS}/sensor_camera_class.cpp)
#add_library(tcp_class SHARED ${DIR_PROJECT}/tcp_class.cpp)
#add_library(message_queue_class SHARED ${DIR_PROJECT}/message_queue_class.cpp)
#add_library(message_queue_class SHARED ${DIR_COMMON}/message_queue_class.cpp)
#add_library(tcp_server SHARED ${DIR_PC_LIBS}/tcp_server.cpp)

add_executable(test_hello_lib ${PROJECT_SOURCES} )
#add_executable(2_concurrent_access_class_mutex ${PROJECT_SOURCES} )
#add_executable(3_concurrent_access_queue ${PROJECT_SOURCES} )
#add_executable(messaging_framework ${PROJECT_SOURCES} )


#target_link_libraries (sensor_camera_class sensor_base_class)
#target_link_libraries (test_sensor_camera test_common_stuff)


message ("\n\nlink the unit test")
target_link_libraries (test_hello_lib ${ALL_PROJECT_LIBS})

# target_link_libraries (cv_test opencv_core opencv_highgui opencv_imgproc opencv_video opencv_videoio)

#target_link_libraries (tcp_server common_stuff message_class message_queue_class)

#target_link_libraries (1_concurrent_access ${ALL_PROJECT_LIBS})
#target_link_libraries (3_concurrent_access_queue ${ALL_PROJECT_LIBS})
#target_link_libraries (messaging_framework ${ALL_PROJECT_LIBS})

