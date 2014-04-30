#!/bin/bash
mkdir -p py
mkdir -p cpp
mkdir -p nano
protoc --plugin=protoc-gen-nanopb=${NANOPB_HOME}/generator/protoc-gen-nanopb --nanopb_out=nano simple.proto --python_out=py --cpp_out=cpp
