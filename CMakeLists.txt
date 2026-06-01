cmake_minimum_required(VERSION 3.30)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(DMG++)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    Boost
    SYSTEM # Disable warnings in headers
    URL https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.zip
)
set(BOOST_INCLUDE_LIBRARIES static_string)

FetchContent_Declare(
    SDL3
    SYSTEM # Disable warnings in headers
    URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-3.4.10.zip
)

set(CMAKE_CXX_FLAGS "-w") # Disable warnings for fetched content
FetchContent_MakeAvailable(Boost SDL3)
link_libraries(Boost::static_string SDL3::SDL3)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-c23-extensions --embed-dir=${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -glldb -gfull")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Og -glldb -DNDEBUG")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)