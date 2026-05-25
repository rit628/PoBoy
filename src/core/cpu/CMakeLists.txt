file(GLOB_RECURSE SRC *.cpp)
add_library(cpu ${SRC})
target_include_directories(cpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cpu PUBLIC mmu)