include_directories(common)

add_subdirectory(core)
add_subdirectory(frontend)
add_subdirectory(util)

add_executable(PoBoy main.cpp)
target_link_libraries(PoBoy PRIVATE core gui)