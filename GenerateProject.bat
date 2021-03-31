mkdir build
cd build
cmake .. -DASSIMP_BUILD_STATIC_LIB=ON -T v141 -A Win32
TIMEOUT 30