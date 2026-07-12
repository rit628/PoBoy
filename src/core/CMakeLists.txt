add_subdirectory(imu)
add_subdirectory(ppu)
add_subdirectory(apu)
add_subdirectory(mmu)
add_subdirectory(cpu)

add_library(core DMG.cpp)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core PUBLIC imu ppu apu mmu cpu)