file(GLOB_RECURSE SRC *.cpp)
add_library(cmu ${SRC})
target_include_directories(cmu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})