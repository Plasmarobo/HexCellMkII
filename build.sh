#!/bin/bash

mkdir -p build
pushd build
make -f ../bootloader/Makefile
make -f ../app/Makefile
popd