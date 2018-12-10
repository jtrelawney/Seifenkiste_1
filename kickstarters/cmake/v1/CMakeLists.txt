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

# to check flags run : make --trace


# if unchanged the messages print
#message("CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}")
#message("CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}")
#CMAKE_CXX_FLAGS_DEBUG is -g
#CMAKE_CXX_FLAGS_RELEASE is -O3 -DNDEBUG

# configure the flags
# CMAKE_CXX_FLAGS is applied to both compiler and linker
# while add_definitions is applied only to compiler

# add flags to both release and debug: -std=c++11 -Wall -pedantic -Wextra -Werror -pthread
#add_definitions(-std=c++11 -Wall -pedantic -Wextra -Werror -pthread)
#or
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic -Wextra -Werror -pthread")
# add flags to only debug -g -Og  (valgrind, profiling etc)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Og -pg")
# add flags to only rekease -O optimizer level 3 and no debugsymbols etc)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")


#SET(GCC_ADDITIONAL_FLAGS "-std=c++11 -Wall -pedantic -Wextra -Werror -pthread -g -Og")

message("\nThe compiler flags are set to")
message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")
message("CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}")

#home dir doesnt change when the subdir is called
message("\ncurrent home dir is ${CMAKE_HOME_DIRECTORY}")
#current source dir will change to local when the subdir is called
message("current source dir is ${CMAKE_CURRENT_SOURCE_DIR}")
#source dir is top of root when several layers are called
message("source dir is ${CMAKE_SOURCE_DIR}")


# the output directories are set -> all code and libs go there
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
message("library output dir is ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/run)
message("runtime output dir is ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")


#this triggers the deteciont and check process
project(test_cmake)

# playing with options, 2 methods, both work with if,then,endif
option(USE_CALLER_CONFIG "Use the configuration settings of the caller" ON)
#set(USE_CALLER_CONFIG OFF)
message("\ncaller config set to = ${USE_CALLER_CONFIG}")

#[[
this works when calling the subdirs, but in when the cmake scripts in the subdirs are called they don't know the variable and fail
hence it is better to use the option above
set(MY_SCOPE "BUILD_ALL_LIBS")
message("\nThe build scope is ${MY_SCOPE}")
]]

# for hello world test example
set(PROJECT_SOURCES test.cpp)
set(PROJECT_LIBS hello_lib)

# hwere we need the location of the library
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/common )


# test what happens in subdir
add_subdirectory(common)

# build the test
add_executable(test_hello ${PROJECT_SOURCES} )
target_link_libraries (test_hello ${PROJECT_LIBS})


