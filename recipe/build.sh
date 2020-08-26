mkdir build
cd build
export ISISROOT=$PWD
cmake -GNinja -DJP2KFLAG=OFF -Dpybindings=OFF -DKAKADU_INCLUDE_DIR=/isisData/kakadu -DbuildTests=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PREFIX ../isis
ninja install
