file(GLOB_RECURSE SRC *.cpp)
add_library(imu ${SRC})
target_include_directories(imu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})