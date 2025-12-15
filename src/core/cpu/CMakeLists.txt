file(GLOB_RECURSE SRC *.cpp)
add_library(cpu ${SRC})
target_include_directories(cpu PUBLIC ../mmu)