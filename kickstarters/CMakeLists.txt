cmake_minimum_required(VERSION 3.0)

project(kickstarters)

SET(GCC_COVERAGE_COMPILE_FLAGS "-Wall -std=c++11")
SET(GCC_COVERAGE_LINK_FLAGS    "-pthread -lrt")

#SET(GCC_COVERAGE_LINK_FLAGS    "-lrt")


SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")





#add_subdirectory(timers)

#include_directories(src src/tcp_reader src ../common)

#add_library(tcp_server SHARED src/tcp_reader/tcp_server.cpp)
#add_library(message_class SHARED ../common/message_class.cpp)
#add_library(message_queue SHARED ../common/message_queue.cpp)

#this didn't work, the library always ended up in build
#install(TARGETS tcp_client DESTINATION ./lib)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "..")

#set(PROJECT_SOURCES ./timers/complex_timer.cpp) #src/tcp_reader/tcp_server.cpp src/tcp_reader/tcp_server.h)

#target_link_libraries(PC_cockpit message_queue)
#target_link_libraries(PC_cockpit message_class)
#target_link_libraries(PC_cockpit tcp_server)


#add_executable(tcp_reader src/tcp_reader/tcp_reader.cpp ${PROJECT_SOURCES})
#target_link_libraries(tcp_reader tcp_server)
#target_link_libraries(tcp_reader message_class)



#### timer examples

add_executable(timer_test timers/timer_read_info.cpp ${PROJECT_SOURCES})
add_executable(simple_timer_test timers/simple_timer.cpp ${PROJECT_SOURCES})



message(STATUS "building complex_timer and need to use the rt -library")

find_library(
    LIB_I_NEED rt
    HINTS "path_to_library"
)

message(STATUS "Found 'external_lib' at: ${LIB_I_NEED}")
add_executable(complex_timer_test timers/complex_timer.cpp)
target_link_libraries (complex_timer_test ${LIB_I_NEED})



#### thread examples

add_executable(thread_test threads/threads.cpp)


