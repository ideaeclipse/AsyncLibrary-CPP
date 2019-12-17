cmake_minimum_required(VERSION 3.12)
project(cpp_async)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)

add_executable(cpp_async main.cpp)

target_link_libraries(cpp_async Threads::Threads)
