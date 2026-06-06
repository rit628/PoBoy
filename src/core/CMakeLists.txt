add_subdirectory(mmu)
add_subdirectory(timer)
add_subdirectory(cpu)
add_subdirectory(ppu)

add_library(core DMG.cpp)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core PUBLIC mmu timer cpu ppu)