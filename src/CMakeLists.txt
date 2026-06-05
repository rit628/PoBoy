include_directories(common)

add_subdirectory(core)
add_subdirectory(util)

add_executable(simpleDMG main.cpp)
target_link_libraries(simpleDMG PUBLIC soc)