cmake -B build_pc1 -G Ninja -DCMAKE_BUILD_TYPE=Release -DUSE_GLES3=ON -DPLATFORM_DESKTOP=ON -DZLIB_USE_STATIC_LIBS=ON
cmake --build build_pc1
cd build_pc1
New_mGBA.exe -v -d
cd ..