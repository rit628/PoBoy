set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(TARGET_TRIPLE x86_64-pc-windows-msvc)

set(CMAKE_C_COMPILER_TARGET ${TARGET_TRIPLE})
set(CMAKE_CXX_COMPILER_TARGET ${TARGET_TRIPLE})

add_compile_options(-Xmicrosoft-windows-sys-root $ENV{WIN_SYSROOT})
add_link_options(-Xmicrosoft-windows-sys-root $ENV{WIN_SYSROOT})

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")