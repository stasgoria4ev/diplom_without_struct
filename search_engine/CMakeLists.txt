cmake_minimum_required(VERSION 3.24)
project(search_engine VERSION 1.0)

#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD 17)

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(nlohmann_json)

add_executable(search_engine main.cpp InvertedIndex.h SearchServer.h ConverterJSON.h)

target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)

add_subdirectory(Google_tests)