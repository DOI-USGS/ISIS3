mkdir build
cd build
export ISISROOT=$PWD
cmake -GNinja -DJP2KFLAG=ON -Dpybindings=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PREFIX -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis
ninja install
