include_directories(common)

add_subdirectory(core)
add_subdirectory(frontend)
add_subdirectory(util)

add_executable(PoBoy WIN32 main.cpp)
target_link_libraries(PoBoy PRIVATE core gui)