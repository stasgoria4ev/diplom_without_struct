# 'Google_test' is the subproject name
project(Google_tests)

set(gtest_disable_pthreads on)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

enable_testing()

# 'Google_Tests_run' is the target name
# 'test1.cpp test2.cpp' are source files with tests

#add_executable(Google_Tests_run gtest.cpp ../main.cpp)
add_executable(Google_Tests_run gtest.cpp)
#add_executable(gtests_test ${Sources} ${Testing_cpp})

target_link_libraries(Google_Tests_run gtest gtest_main nlohmann_json::nlohmann_json)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
include(GoogleTest)