cmake -B build_switch22 -DCMAKE_BUILD_TYPE=Release -DPLATFORM_SWITCH=ON &&
make -C build_switch22 New_mGBA.nro -j$(nproc)