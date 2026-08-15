file(GLOB_RECURSE SRC *.cpp)
add_library(core ${SRC})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core PUBLIC cart imu ppu apu bus cpu)