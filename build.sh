cmake -B build -DCMAKE_BUILD_TYPE=Release -DPLATFORM_SWITCH=ON -DUSE_DEKO3D=ON &&
make -C build New_mGBA.nro -j$(nproc)