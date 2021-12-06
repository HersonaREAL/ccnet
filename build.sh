#!/bin/bash

# TODO 安装依赖

# cmake 
cmake . -B build
cp build/compile_commands.json .

# TODO 全核心编译
make -C build