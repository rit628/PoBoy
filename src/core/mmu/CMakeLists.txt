file(GLOB_RECURSE SRC *.cpp)
add_library(mmu ${SRC})
target_include_directories(mmu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mmu PUBLIC imu apu ppu)