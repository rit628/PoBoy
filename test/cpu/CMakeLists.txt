file(GLOB_RECURSE SRC *.cpp)
add_executable(test_cpu ${SRC})
target_include_directories(test_cpu PRIVATE ${CMAKE_SOURCE_DIR}/src/common)
target_link_libraries(test_cpu PRIVATE cpu Boost::json)