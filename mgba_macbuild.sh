cd mgbalib
rm *.dylib
cd ..
cd library/lib/extern/mgba
brew install cmake ffmpeg libzip qt5 sdl2 libedit pkg-config
mkdir build
cd build
rm -rf *
cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_PREFIX_PATH=`brew --prefix qt5` ..
make
cd ../../../../../
cp library/lib/extern/mgba/build/*.dylib mgbalib/
# cmake -B build_macos -DCMAKE_BUILD_TYPE=Release -DPLATFORM_DESKTOP=ON -DBUNDLE_MACOS_APP=ON -DUSE_GLFW=ON
# make -C build_macos -j$(sysctl -n hw.ncpu)
# open ./build_macos/New_mGBA.app