#!/bin/sh

cd $(dirname $0)

mkdir build
mkdir debug
mkdir bin

cd build
cmake -DCMAKE_BUILD_TYPE=Release ..

cd ../debug
cmake -DCMAKE_BUILD_TYPE=Debug ..

cd ../build

make
cd ..
cd bin

cp ../build/gen-concepts .

