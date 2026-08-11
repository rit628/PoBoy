file(GLOB_RECURSE SRC *.cpp)
add_library(cart ${SRC})
target_include_directories(cart PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})