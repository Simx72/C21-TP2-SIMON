#!/bin/bash

set -euxo pipefail

mkdir -p build/package

ROOT="build/package"

cp main.cpp   "$ROOT/main.cpp"
cp cvm_23.cpp "$ROOT/cvm_23.cpp"
cp cvm_23.h   "$ROOT/cvm_23.h"
cp build/GestionBanque.exe "$ROOT/GestionBanque.exe"

cd build

zip -r package.zip package

