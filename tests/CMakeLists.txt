cmake_minimum_required(VERSION 3.12)
project(cpp_async)

  set(CPM_DIR "${CMAKE_CURRENT_BINARY_DIR}/cpm-packages" CACHE TYPE STRING)
  find_package(Git)
if(NOT GIT_FOUND)
	message(FATAL_ERROR "CPM requires Git.")
endif()
if (NOT EXISTS ${CPM_DIR}/CPM.cmake)
  message(STATUS "Cloning repo (https://github.com/iauns/cpm)")
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" clone https://github.com/iauns/cpm ${CPM_DIR}
		RESULT_VARIABLE error_code
    OUTPUT_QUIET ERROR_QUIET)
  if(error_code)
  	message(FATAL_ERROR "CPM failed to get the hash for HEAD")
	endif()
endif()
include(${CPM_DIR}/CPM.cmake)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)

add_executable(cpp_async main.cpp)

target_link_libraries(cpp_async Threads::Threads)

CPM_Finish()
