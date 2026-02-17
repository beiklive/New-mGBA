cmake -B build_macos -DCMAKE_BUILD_TYPE=Release -DPLATFORM_DESKTOP=ON -DBUNDLE_MACOS_APP=ON -DUSE_GLFW=ON
make -C build_macos -j$(sysctl -n hw.ncpu)