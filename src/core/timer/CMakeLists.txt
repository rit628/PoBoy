file(GLOB_RECURSE SRC *.cpp)
add_library(timer ${SRC})
target_include_directories(timer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(timer PUBLIC mmu)