file(GLOB_RECURSE SRC *.cpp)
add_library(bus ${SRC})
target_include_directories(bus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bus PUBLIC imu apu ppu)