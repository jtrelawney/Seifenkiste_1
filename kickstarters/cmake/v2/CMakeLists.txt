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
message("${BoldGreen}\n\ncmake script for cmake test main${ColourReset}")
#message("${Yellow}\n\ncmake script for cmake test main${ColourReset}")
#message("${BoldRed}\n\ncmake script for cmake test main${ColourReset}")

# many hints taken from how to use flags
#https://ecrafter.wordpress.com/2012/04/24/cmake-tutorial-part-2/

# 2 build types: release and debug, call cmake with flags to invoke the desired type
#cmake . -DCMAKE_BUILD_TYPE=Release
#cmake . -DCMAKE_BUILD_TYPE=Debug

message("\nThe build type is DCMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")


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

# put all libs in lib direoctory
if (CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib_release)
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
endif()
message("library output dir is ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

# put all exe in run directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/run)
message("runtime output dir is ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# this triggers the detecion and check process
project(test_cmake)

# define include locations
include_directories(common)
# define link locations -> for own libraries it is a central repository
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
# set libraries to use for this project
set(MY_LIBS hello_lib)
set(GENERAL_LIBS rt)
set(OPENCV_LIBS opencv_core opencv_highgui opencv_imgproc opencv_video opencv_videoio)
set(ALL_PROJECT_LIBS ${GENERAL_LIBS} ${MY_LIBS} ${OPENCV_LIBS} )
message ("\n\nsetting libraries as :\n" ${ALL_PROJECT_LIBS} "\n")

# build library
add_library(hello_lib SHARED common/hello_lib.cpp)
set(PROJECT_LIBS hello_lib)
add_executable(hello_unit_test test.cpp )
target_link_libraries (hello_unit_test ${ALL_PROJECT_LIBS})

# build the hello world test example
add_executable(test_hello_lib common/test_hello_lib.cpp )
target_link_libraries (test_hello_lib ${ALL_PROJECT_LIBS})
