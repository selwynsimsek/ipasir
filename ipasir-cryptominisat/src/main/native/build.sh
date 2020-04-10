#!/bin/bash

set -e
set -x

rm -rf bin/crypto*
rm -rf bin/cusp*

rm -rf build
mkdir build

(
cd build || exit
cmake -DENABLE_PYTHON_INTERFACE=OFF -DNOVALGRIND=ON -DNOZLIB=ON -DONLY_SIMPLE=ON -DSTATICCOMPILE=ON -DIPASIR=ON -MIT=ON -DCMAKE_BUILD_TYPE=Release $@ ..
make -j4
)
