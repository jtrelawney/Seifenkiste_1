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
message("${BoldGreen}\n\ncmake script for cmake project : build all libs${ColourReset}")
#message("${Yellow}\n\ncmake script for cmake test main${ColourReset}")
#message("${BoldRed}\n\ncmake script for cmake test main${ColourReset}")


# many hints taken from how to use flags
#https://ecrafter.wordpress.com/2012/04/24/cmake-tutorial-part-2/

# 2 build types: release and debug, call cmake with flags to invoke the desired type
#cmake .. -DCMAKE_BUILD_TYPE=Release
#cmake .. -DCMAKE_BUILD_TYPE=Debug

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()
message("\nThe build type is DCMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

# this triggers the detecion and check process
project(all_libs)

# define common and debug/release compiler flags
# to check flags run : make --trace

# check if this script was called with params given, if not use own params
if(USE_CALLER_CONFIG)
    message("\ncaller config = ${USE_CALLER_CONFIG}")
else()
    message("\ncallerconfig = ${USE_CALLER_CONFIG}  => use own config")
    # add flags to both release and debug: -std=c++11 -Wall -pedantic -Wextra -Werror -pthread
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic -Wextra -Werror -pthread")
    # add flags to only debug -g -Og  (valgrind, profiling etc)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Og -pg")
    # add flags to only rekease -O optimizer level 3 and no debugsymbols etc)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif(USE_CALLER_CONFIG)

message("\nThe compiler flags are set to")
message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")
message("CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}")

# print some directory settings
#home dir doesnt change when the subdir is called
message("\ncurrent home dir is ${CMAKE_HOME_DIRECTORY}")
#current source dir will change to local when the subdir is called
message("current source dir is ${CMAKE_CURRENT_SOURCE_DIR}")
#source dir is top of root when several layers are called
message("source dir is ${CMAKE_SOURCE_DIR}")

# target directory for libs
if (CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib_release)
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib_debug)
endif()
message("library output dir is ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

# target directory for lib executables (unit tests)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib_run)
message("runtime output dir is ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
# target location of library headers
set(MY_INCLUDE_TARGET_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib_include)
message("include target directory is ${MY_INCLUDE_TARGET_DIRECTORY}")

# define include locations
# if a new class is created append here
include_directories(common message_class sensor_class message_queue_class tcp_class cockpit_class)
# define link locations -> for own libraries it is a central repository
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
# set libraries to use for this project
set(MY_LIBS common_stuff)
set(GENERAL_LIBS rt)
set(OPENCV_LIBS opencv_core opencv_highgui opencv_imgproc opencv_video opencv_videoio)
set(ALL_PROJECT_LIBS ${GENERAL_LIBS} ${MY_LIBS} ${OPENCV_LIBS} )
message ("\n\nsetting libraries as :\n" ${ALL_PROJECT_LIBS} "\n")

# initialize library list with the external libs, then keep adding the new libraries and use the updated list for linking at that point
list(APPEND PROJECT_LIBS rt)
list(APPEND PROJECT_LIBS opencv_core opencv_highgui opencv_imgproc opencv_video opencv_videoio)

# keep a second list with alls subdirs, this is used later to install the includes
#list(APPEND PROJECT_SUB_DIRS /message_class /message_queue_class)

# build common library
list(APPEND PROJECT_SUB_DIRS /common)
add_library(common_stuff SHARED common/common.cpp)
add_executable(test_common_stuff common/test_common_stuff.cpp )

list(APPEND PROJECT_LIBS common_stuff)
target_link_libraries (test_common_stuff ${PROJECT_LIBS})

# build message library
list(APPEND PROJECT_SUB_DIRS /message_class)
add_library(address_class SHARED message_class/address_class.cpp)
add_library(buffer_class SHARED message_class/buffer_class.cpp)
add_library(message_class SHARED message_class/message_class.cpp)

#add_executable(buffer_class_unit_test message_class/buffer_class_unit_test.cpp )
add_executable(test_message_class message_class/test_message_class.cpp message_class/buffer_and_address_class_unit_test.cpp )

list(APPEND PROJECT_LIBS address_class buffer_class)
target_link_libraries (message_class ${PROJECT_LIBS} )
list(APPEND PROJECT_LIBS message_class)
#target_link_libraries (buffer_class_unit_test ${PROJECT_LIBS})
target_link_libraries (test_message_class ${PROJECT_LIBS})

# build sensor library
list(APPEND PROJECT_SUB_DIRS /message_class /sensor_class)
add_library(sensor_base_class SHARED sensor_class/sensor_base_class.cpp)
add_library(sensor_camera_class SHARED sensor_class/sensor_camera_class.cpp)
add_executable(test_sensor_camera sensor_class/test_sensor_camera.cpp )

target_link_libraries (sensor_base_class ${PROJECT_LIBS})
list(APPEND PROJECT_LIBS sensor_base_class)
target_link_libraries (sensor_camera_class ${PROJECT_LIBS})
list(APPEND PROJECT_LIBS sensor_base_class sensor_camera_class)
target_link_libraries (test_sensor_camera ${PROJECT_LIBS})

# build message queue library
list(APPEND PROJECT_SUB_DIRS /message_queue_class)
add_library(communication_class SHARED message_queue_class/process_communication_class.cpp)
add_library(message_queue_class SHARED message_queue_class/message_queue_class.cpp)
add_executable(test_message_queue_class message_queue_class/test_message_queue_class.cpp)

target_link_libraries(communication_class ${PROJECT_LIBS})
list(APPEND PROJECT_LIBS communication_class)
target_link_libraries(message_queue_class ${PROJECT_LIBS})
list(APPEND PROJECT_LIBS message_queue_class communication_class)
target_link_libraries (test_message_queue_class ${PROJECT_LIBS})

# build tcp library
list(APPEND PROJECT_SUB_DIRS /tcp_class)
add_library(tcp_class SHARED tcp_class/tcp_class.cpp)
add_library(tcp_client_class SHARED tcp_class/tcp_client.cpp)
add_library(tcp_server_class SHARED tcp_class/tcp_server.cpp)
add_executable(test_tcp_class tcp_class/test_tcp_class.cpp )

target_link_libraries (tcp_server_class tcp_class message_class buffer_class message_queue_class)
target_link_libraries (tcp_client_class tcp_class message_class buffer_class)
list(APPEND PROJECT_LIBS tcp_server_class tcp_client_class)
target_link_libraries (test_tcp_class ${PROJECT_LIBS})

# build cockpit library
list(APPEND PROJECT_SUB_DIRS /cockpit_class)
add_library(cockpit_class SHARED cockpit_class/cockpit_class.cpp)
add_executable(test_cockpit_class cockpit_class/test_cockpit_class.cpp )

target_link_libraries (cockpit_class ${PROJECT_LIBS})
list(APPEND PROJECT_LIBS cockpit_class )
target_link_libraries (test_cockpit_class ${PROJECT_LIBS} )


# finally where are we
message ("\nall PROJECT libs = " ${PROJECT_LIBS})

#[[
#kinda works but how can it be consistent when stuff changes
add_custom_command(
    WORKING_DIRECTORY ${MY_INCLUDE_TARGET_DIRECTORY}
    OUTPUT ${MY_INCLUDE_TARGET_DIRECTORY}/move_my_shit_output.log
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/move_my_shit.sh
)

add_custom_target(
  BuildDocs ALL
  DEPENDS move_my_shit_output.log
)
]]


# libs are done, now move the headers
message ( "\ninstalling header files from " ${CMAKE_CURRENT_SOURCE_DIR} " into " ${MY_INCLUDE_TARGET_DIRECTORY})
FOREACH(subdir ${PROJECT_SUB_DIRS})
    message ("subdir = " ${CMAKE_CURRENT_SOURCE_DIR}${subdir}/)
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}${subdir}/" # source directory
            DESTINATION ${MY_INCLUDE_TARGET_DIRECTORY} # target directory
            FILES_MATCHING # install only matched files
            PATTERN "*.h" # select header files
            PATTERN "build" EXCLUDE
    )
ENDFOREACH()



