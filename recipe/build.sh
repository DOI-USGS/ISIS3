mkdir build
cd build
export ISISROOT=$PWD
export CMAKE_PREFIX_PATH="$PREFIX"
cmake -GNinja \
  -DJP2KFLAG=ON \
  -Dpybindings=ON \
  -DKAKADU_INCLUDE_DIR=/Users/krodriguez/kakadu_7_9 \
  -DbuildTests=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  -DCMAKE_PREFIX_PATH=$PREFIX \
  -DPython3_EXECUTABLE=$PYTHON \
  -DPython_EXECUTABLE=$PYTHON \
  -DPython3_ROOT_DIR=$PREFIX \
  -DPython_ROOT_DIR=$PREFIX \
  -DCMAKE_FIND_FRAMEWORK=LAST \
  ../isis
ninja install
