cmake -S . -B build -DCMAKE_BUILD_TYPE=${1^}
cmake --build build -j$(nproc)