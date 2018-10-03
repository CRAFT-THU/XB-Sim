cmake_minimum_required(VERSION 3.7)
project(simulator_mac)

find_package(SystemCLanguage CONFIG REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++ -lstdc++ -std=c++11")

set(INC_DIR /Users/liuhe/Workspace/systemc-2.3.2/include)
set(LINK_DIR /Users/liuhe/Workspace/systemc-2.3.2/lib)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(systemc)

set(SOURCE_FILES
        config.h
        numgen.h
        crossbar.h
        stage_conv_1.h
        stage_conv_2.h
        stage_conv_3.h
        stage_conv_4.h
        stage_conv_5.h
        stage_conv_6.h
        stage_conv_7.h
        stage_conv_8.h
        stage_conv_9.h
        stage_conv_10.h
        stage_conv_11.h
        stage_conv_12.h
        stage_conv_13.h
        stage_conv_14.h
        stage_conv_15.h
        conv_buffer_1.h
        conv_buffer_2.h
        conv_buffer_3.h
        conv_buffer_4.h
        conv_buffer_5.h
        conv_buffer_6.h
        conv_buffer_7.h
        conv_buffer_8.h
        conv_buffer_9.h
        conv_buffer_10.h
        conv_buffer_11.h
        conv_buffer_12.h
        conv_buffer_13.h
        conv_buffer_14.h
        stage_linear_16.h
        stage_linear_17.h
        linear_buffer_15.h
        linear_buffer_16.h
        display.h
        main.cpp)
add_executable(simulator_mac ${SOURCE_FILES})
target_link_libraries(simulator_mac systemc)