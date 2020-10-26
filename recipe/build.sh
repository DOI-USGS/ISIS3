mkdir build
cd build
export ISISROOT=$PWD
cmake -GNinja -DJP2KFLAG=ON -Dpybindings=OFF -DbuildTests=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PREFIX ../isis
ninja install
