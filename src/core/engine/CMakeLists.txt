file(GLOB_RECURSE SRC *.cpp)
add_library(engine ${SRC})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine PUBLIC mmu cpu)