# Build project
rm -rf build
mkdir build
cd build

export OPENSSL_ROOT_DIR=${CI_OPENSSL3_DIR}
export CMAKE_PREFIX_PATH=${CI_QT_PATH}:$CMAKE_PREFIX_PATH
export LD_LIBRARY_PATH=.:${CI_QT_PATH}/lib
cmake ..
make
