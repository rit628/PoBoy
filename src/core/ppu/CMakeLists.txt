file(GLOB_RECURSE SRC *.cpp)
add_library(ppu ${SRC})
target_include_directories(ppu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ppu PUBLIC imu)