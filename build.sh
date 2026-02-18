cmake -B build_switch -DCMAKE_BUILD_TYPE=Release -DPLATFORM_SWITCH=ON -DUSE_DEKO3D=ON &&
make -C build_switch New_mGBA.nro -j$(nproc)