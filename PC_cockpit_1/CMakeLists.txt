cmake_minimum_required(VERSION 3.0)

project(PC_cockpit)

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++11")
SET(GCC_COVERAGE_LINK_FLAGS    "-pthread")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")


#add_subdirectory(./src)

include_directories(src src/tcp_reader)

add_library(tcp_server SHARED src/tcp_reader/tcp_server.cpp)
add_library(message_class SHARED src/common/message_class.cpp)
add_library(message_queue SHARED src/common/message_queue.cpp)

#this didn't work, the library always ended up in build
#install(TARGETS tcp_client DESTINATION ./lib)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "..")

set(PROJECT_SOURCES src/tcp_reader/tcp_server.cpp src/tcp_reader/tcp_server.h)

add_executable(PC_cockpit src/PC_cockpit_1.cpp ${PROJECT_SOURCES})
target_link_libraries(PC_cockpit message_queue)
target_link_libraries(PC_cockpit message_class)
target_link_libraries(PC_cockpit tcp_server)


add_executable(tcp_reader src/tcp_reader/tcp_reader.cpp ${PROJECT_SOURCES})
target_link_libraries(tcp_reader tcp_server)
target_link_libraries(tcp_reader message_class)

