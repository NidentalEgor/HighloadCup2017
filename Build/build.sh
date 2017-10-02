#!/bin/bash

build_type=$1

if [[ "$1" = "Debug" ]] || [[ "$1" = "Release" ]]
then
  build_type=$1
elif [[ $1 == "" ]]; then
  build_type="Release"
else
  echo "Error! Build type is invalid!"; exit $ERRCODE;
fi
mkdir Linux
cd Linux
mkdir $build_type
cd $build_type
cmake -DCMAKE_BUILD_TYPE=$build_type ../../..
make