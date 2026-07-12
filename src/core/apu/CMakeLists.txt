file(GLOB_RECURSE SRC *.cpp)
add_library(apu ${SRC})
target_include_directories(apu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(apu PUBLIC imu)