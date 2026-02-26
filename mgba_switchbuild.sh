cd mgbalib
rm *.a
cd ..
cd library/lib/extern/mgba
mkdir build
cd build
rm -rf *
cmake -DCMAKE_TOOLCHAIN_FILE=../src/platform/switch/CMakeToolchain.txt ..
make
cd ../../../../../
cp library/lib/extern/mgba/build/*.a mgbalib/
# cmake -B build_macos -DCMAKE_BUILD_TYPE=Release -DPLATFORM_DESKTOP=ON -DBUNDLE_MACOS_APP=ON -DUSE_GLFW=ON
# make -C build_macos -j$(sysctl -n hw.ncpu)
# open ./build_macos/New_mGBA.app