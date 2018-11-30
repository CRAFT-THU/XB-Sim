cmake_minimum_required(VERSION 3.5)
project(simulator-windows)

find_package(SystemCLanguage CONFIG REQUIRED)

option(USE_CUDA "Default option USE_CUDA is off" off)
message(STATUS "computation use cuda switch: " ${USE_CUDA})
if(${USE_CUDA} STREQUAL "on")
	find_package(CUDA REQUIRED)
endif(${USE_CUDA} STREQUAL "on")

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++11 -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fopenmp")
if(${USE_CUDA} STREQUAL "on")
	set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS})
endif(${USE_CUDA} STREQUAL "on")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fopenmp -I/usr/local/opt/libomp/include -L/usr/local/opt/libomp/lib -lomp ")

if(${USE_CUDA} STREQUAL "on")
	execute_process(COMMAND python cpp_gen.py --cuda true
					WORKING_DIRECTORY ../templates/)
else()
	execute_process(COMMAND python cpp_gen.py
					WORKING_DIRECTORY ../templates/)
endif(${USE_CUDA} STREQUAL "on")

set(SOURCE_FILES main.cpp)

if(${USE_CUDA} STREQUAL "on")
	set(MY_CUDA_FILES crossbar.cu)
	CUDA_ADD_LIBRARY(MY_CUDA_LIB STATIC ${MY_CUDA_FILES})
	add_executable(simulator-windows ${SOURCE_FILES})
	target_link_libraries(simulator-windows MY_CUDA_LIB /usr/local/cuda-8.0/lib64/libcurand.so)
	target_link_libraries(simulator-windows SystemC::systemc)
else()
	add_executable(simulator-windows ${SOURCE_FILES})
	target_link_libraries(simulator-windows SystemC::systemc)
endif(${USE_CUDA} STREQUAL "on")

