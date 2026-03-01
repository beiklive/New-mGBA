cd mgbalib
rm libmgba*
cd ..
cd library/lib/extern/mgba
mkdir build_pc
cd build_pc
rm -rf *
cmake  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 .. -G "MSYS Makefiles"
make -j$(nproc --ignore=1)
cd ../../../../../
cp library/lib/extern/mgba/build_pc/libmgba* mgbalib/
# cmake -B build_macos -DCMAKE_BUILD_TYPE=Release -DPLATFORM_DESKTOP=ON -DBUNDLE_MACOS_APP=ON -DUSE_GLFW=ON
# make -C build_macos -j$(sysctl -n hw.ncpu)
# open ./build_macos/New_mGBA.app