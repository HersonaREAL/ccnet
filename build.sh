#!/bin/bash

# TODO 安装依赖

# cmake 
cmake . -B build -DCMAKE_BUILD_TYPE=Debug
cp build/compile_commands.json .

# TODO 全核心编译
make -C build -j8