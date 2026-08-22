include($ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake)

add_link_options(-sEXPORTED_RUNTIME_METHODS=[ccall,cwrap] -sEXPORTED_FUNCTIONS=[_pushFileEvent,_pushKeyboardEvent,_main])

set(EXECUTABLE_TARGET ".html")

set(CMAKE_EXECUTABLE_SUFFIX_C ${EXECUTABLE_TARGET})
set(CMAKE_EXECUTABLE_SUFFIX_CXX ${EXECUTABLE_TARGET})

set(CMAKE_LINKER_TYPE DEFAULT)