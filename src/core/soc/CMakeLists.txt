file(GLOB_RECURSE SRC *.cpp)
add_library(soc ${SRC})
target_include_directories(soc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(soc PUBLIC mmu timer cpu ppu)