#!/bin/bash

set -e
set -x

if [ -n "$1" ]; then
  # interpret the parameter as a toolchain file
  OPTIONS="-DCMAKE_TOOLCHAIN_FILE=$1"
fi


rm -rf bin/crypto*
rm -rf bin/cusp*

rm -rf build
mkdir build

(
cd build || exit
cmake -DENABLE_PYTHON_INTERFACE=OFF -DNOVALGRIND=ON -DNOZLIB=ON -DONLY_SIMPLE=ON -DSTATICCOMPILE=ON -DIPASIR=ON -DCMAKE_BUILD_TYPE=Release $OPTIONS ..
make -j4
)
