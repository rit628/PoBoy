include($ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake)

set(CMAKE_EXECUTABLE_SUFFIX_C ".html")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".html")
set(CMAKE_LINKER_TYPE DEFAULT)