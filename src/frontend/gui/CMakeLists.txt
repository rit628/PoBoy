file(GLOB_RECURSE SRC *.cpp)
add_library(gui ${SRC})
target_include_directories(gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gui PUBLIC core SDL3::SDL3)