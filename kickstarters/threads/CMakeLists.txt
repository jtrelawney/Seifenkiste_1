cmake_minimum_required(VERSION 3.0)

project(concurrent_threads)

# compiler flags
#SET(GCC_ADDITIONAL_FLAGS "-std=c++11 -Wall -pedantic -Wextra -Werror -pthread")
#r
SET(GCC_ADDITIONAL_FLAGS "-std=c++11 -Wall -pedantic -Wextra -Werror -pthread -fsanitize=thread -O2 -g")



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
set(DIR_COMMON ${PROJECT_PATH}/lib)
set(DIR_PC_LIBS ${DIR_COMMON}/pc_lib)
set(DIR_PROJECT ${PROJECT_PATH}/PC_cockpit)

message ("\nusing the following source directories for the main and libs:")
message (${DIR_KICKSTARTER})
#message (${PROJECT_PATH})
#message (${DIR_COMMON})
#message (${DIR_PC_LIBS})
#message (${DIR_PROJECT})


# include directories
include_directories( ${DIR_COMMON} ${DIR_PC_LIBS} )
message ("\n\nheader dirs:")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

# set libraries to use for this project
#set(OPENCV_LIBS opencv_core opencv_highgui opencv_imgproc opencv_video opencv_videoio)
#set(MY_LIBS common_stuff message_class tcp_server message_queue_class)
set(GENERAL_LIBS rt)
set(ALL_PROJECT_LIBS ${GENERAL_LIBS} ${MY_LIBS})
message ("\n\nsetting libraries as :\n" ${ALL_PROJECT_LIBS} "\n")


# define project targets, what will be compiled
#set(PROJECT_SOURCES  ${DIR_COMMON}/message_class.cpp ${DIR_PROJECT}/stream_rpi_camera.cpp )

#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/1_concurrent_access.cpp)
#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/2_concurrent_access_class_mutex.cpp)
#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/3_concurrent_access_queue.cpp)
#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/4_concurrent_access_spmc.cpp)
#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/5_queue_spsc_cond_vars.cpp)
#set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/6_queue_spmc_cond_vars.cpp)
set(PROJECT_SOURCES  ${DIR_KICKSTARTER}/threads/7_queue_mpmc_cond_vars.cpp)


message ("\n\nmain target to compile")
message ("${PROJECT_SOURCES}")

# add targets 
#add_library(common_stuff SHARED ${DIR_COMMON}/defs.cpp)
#add_library(message_class SHARED ${DIR_COMMON}/message_class.cpp)
#add_library(message_queue_class SHARED ${DIR_COMMON}/message_queue_class.cpp)
#add_library(tcp_server SHARED ${DIR_PC_LIBS}/tcp_server.cpp)

#add_executable(1_concurrent_access ${PROJECT_SOURCES} )
#add_executable(2_concurrent_access_class_mutex ${PROJECT_SOURCES} )
#add_executable(3_concurrent_access_queue ${PROJECT_SOURCES} )
#add_executable(4_concurrent_access_spmc ${PROJECT_SOURCES} )
#add_executable(5_queue_spsc_cond_vars ${PROJECT_SOURCES} )
#add_executable(6_queue_spmc_cond_vars ${PROJECT_SOURCES} )
add_executable(7_queue_mpmc_cond_vars ${PROJECT_SOURCES} )


#target_link_libraries (message_class common_stuff)
#target_link_libraries (message_queue_class common_stuff message_class)
#target_link_libraries (tcp_server common_stuff message_class message_queue_class)

#target_link_libraries (1_concurrent_access ${ALL_PROJECT_LIBS})
#target_link_libraries (2_concurrent_access_class_mutex ${ALL_PROJECT_LIBS})
#target_link_libraries (3_concurrent_access_queue ${ALL_PROJECT_LIBS})
#target_link_libraries (4_concurrent_access_spmc ${ALL_PROJECT_LIBS})
#target_link_libraries (5_queue_spsc_cond_vars ${ALL_PROJECT_LIBS})
#target_link_libraries (6_queue_spmc_cond_vars ${ALL_PROJECT_LIBS})
target_link_libraries (7_queue_mpmc_cond_vars ${ALL_PROJECT_LIBS})

