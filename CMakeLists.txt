cmake_minimum_required(VERSION 4.0)

project(PoBoy)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/.deps)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    Boost
    SYSTEM # Disable warnings in headers
    URL https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.zip
)
set(BOOST_INCLUDE_LIBRARIES json)

FetchContent_Declare(
    SDL3
    SYSTEM # Disable warnings in headers
    URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-3.4.10.zip
)

option(COMPILE_SST OFF)

if(COMPILE_SST)
    set(OPTIONAL_DEPS Boost)
endif()

set(CMAKE_CXX_FLAGS "-w") # Disable warnings for fetched content
set(SDL_STATIC ON) # Simplifies binary distribution
set(SDL_SHARED OFF)
FetchContent_MakeAvailable(SDL3 ${OPTIONAL_DEPS})

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-c23-extensions --embed-dir=${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -glldb -gfull")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Og -fno-omit-frame-pointer -g -glldb -gfull")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)
if(COMPILE_SST)
    add_subdirectory(test)
endif()