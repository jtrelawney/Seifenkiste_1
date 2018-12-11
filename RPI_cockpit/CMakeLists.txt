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
message("${BoldGreen}\n\ncmake script for cmake : pc cockpit (main)${ColourReset}")
#message("${Yellow}\n\ncmake script for cmake test main${ColourReset}")
#message("${BoldRed}\n\ncmake script for cmake test main${ColourReset}")

# many hints taken from how to use flags
#https://ecrafter.wordpress.com/2012/04/24/cmake-tutorial-part-2/

# 2 build types: release and debug, call cmake with flags to invoke the desired type
#cmake . -DCMAKE_BUILD_TYPE=Release
#cmake . -DCMAKE_BUILD_TYPE=Debug

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()
message("\nThe build type is DCMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

# this triggers the detecion and check process
project(PC_cockpit)

# define common and debug/release compiler flags
# to check flags run : make --trace

# add flags to both release and debug: -std=c++11 -Wall -pedantic -Wextra -Werror -pthread
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic -Wextra -Werror -pthread")
# add flags to only debug -g -Og  (valgrind, profiling etc)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Og -pg")
# add flags to only rekease -O optimizer level 3 and no debugsymbols etc)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

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

# setting directories
set(PROJECT_PATH "/home/pi/projects/Seifenkiste_1")
set(DIR_PROJECT ${PROJECT_PATH}/RPI_cockpit)
set(DIR_LIBS ${PROJECT_PATH}/lib)

message ("\nusing the following source directories for the main and libs:")
message (${PROJECT_PATH})
message (${DIR_PROJECT})
message (${DIR_LIBS})

# the libs are in the subdirectory lib (which is a link to the real lib directory
if (CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIR_LIBS}/lib_release)
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIR_LIBS}/lib_debug)
endif()
message("library dir is ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

# target location of library headers
set(MY_INCLUDE_TARGET_DIRECTORY ${DIR_LIBS}/lib_include)
message("include directory is ${MY_INCLUDE_TARGET_DIRECTORY}")

# set the option so that when building the libs the project settings are used
option(USE_CALLER_CONFIG "Use the configuration settings of the caller" ON)
message("\nin cmake main : caller config set to = ${USE_CALLER_CONFIG}")
message("\n===================================================")

# first do not build the libraries
# this directory needs to exist , as mentioned above as a link to the project lib directory
# add_subdirectory(lib)

# set libraries to use for this project
set(OPENCV_LIBS opencv_core opencv_highgui opencv_imgproc opencv_video opencv_videoio)
set(GENERAL_LIBS rt)
set(MY_LIBS common_stuff buffer_class address_class message_class sensor_base_class sensor_camera_class communication_class message_queue_class)
set(ALL_PROJECT_LIBS ${GENERAL_LIBS} ${MY_LIBS} ${OPENCV_LIBS})
message ("\n\nsetting libraries as :\n" ${ALL_PROJECT_LIBS} "\n")

# define link locations -> for own libraries it is a central repository
include_directories(${MY_INCLUDE_TARGET_DIRECTORY})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message("include directory is ${MY_INCLUDE_TARGET_DIRECTORY}")
message("library dir is ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

# build the cockpit
add_executable(rpi_cockpit_main RPI_cockpit_main_process.cpp)
target_link_libraries (rpi_cockpit_main ${ALL_PROJECT_LIBS} tcp_client_class )
#add_executable(pc_cockpit_test PC_cockpit_test_process.cpp)
#target_link_libraries (pc_cockpit_test ${ALL_PROJECT_LIBS} tcp_client_class)

