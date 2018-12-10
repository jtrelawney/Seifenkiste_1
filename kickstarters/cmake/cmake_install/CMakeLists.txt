cmake_minimum_required(VERSION 3.0)

# print some directory settings
#home dir doesnt change when the subdir is called
message("\ncurrent home dir is ${CMAKE_HOME_DIRECTORY}")
#current source dir will change to local when the subdir is called
message("current source dir is ${CMAKE_CURRENT_SOURCE_DIR}")
#source dir is top of root when several layers are called
message("source dir is ${CMAKE_SOURCE_DIR}")

# target directory for libs
if (CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib_release)
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib_debug)
endif()
message("library output dir is ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

# target directory for lib executables (unit tests)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib_run)
message("runtime output dir is ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
# target location of library headers
set(MY_INCLUDE_TARGET_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib_include)
message("include target directory is ${MY_INCLUDE_TARGET_DIRECTORY}")

# this triggers the detecion and check process
project(install_lib)

# finally where are we
message ("\nPROJECT libs = " ${PROJECT_LIBS})

list(APPEND PROJECT_SUB_DIRS /common_1)
list(APPEND PROJECT_SUB_DIRS /common_2)

# libs are done, now move the headers
message ( "\ninstalling header files ")
message ( "from dirs in " ${CMAKE_CURRENT_SOURCE_DIR})
message ( "to " ${MY_INCLUDE_TARGET_DIRECTORY})

project(myproject)

add_library(mylib1 SHARED common_1/test1.cpp)
target_include_directories(mylib1 PUBLIC common_1)
set_target_properties(mylib1 PROPERTIES PUBLIC_HEADER "common_1/test1.h")

add_executable(unit_test_mylib1 common_1/unit_test_mylib1.cpp)
target_include_directories(unit_test_mylib1 PUBLIC common_1)
target_link_libraries (unit_test_mylib1 mylib1)

add_library(mylib2 SHARED common_2/test2.cpp)
target_include_directories(mylib2 PUBLIC common_2)
set_target_properties(mylib2 PROPERTIES PUBLIC_HEADER "common_2/test2.h")

add_executable(unit_test_mylib2 common_2/unit_test_mylib2.cpp)
target_include_directories(unit_test_mylib2 PUBLIC common_2)
target_link_libraries (unit_test_mylib2 mylib2)

INSTALL(TARGETS mylib1
        LIBRARY DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        PUBLIC_HEADER DESTINATION ${MY_INCLUDE_TARGET_DIRECTORY}
        RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

INSTALL(TARGETS mylib2
        LIBRARY DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        PUBLIC_HEADER DESTINATION ${MY_INCLUDE_TARGET_DIRECTORY}
        RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# this triggers the detecion and check process
project(install_proj)
add_executable(decinc common_main/decinc.cpp)
target_include_directories(decinc PUBLIC common_1 common_2)
target_link_libraries (decinc mylib1 mylib2)
#set_property(TARGET decinc PROPERTY FOLDER ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# this doesn't work in install mode, it complains the taarget file doesn't exist in the target directory, and why would it, it is supposed to be copied there
# so some messup with the filepath which I don't understand yet
#INSTALL(TARGETS decinc RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
