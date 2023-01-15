#! /bin/bash

set -e 

rm -rf `pwd`/bind/*
cd `pwd`/bind &&
    cmake .. &&
    make

cd ..
cp -r `pwd`/src/include `pwd`/lib