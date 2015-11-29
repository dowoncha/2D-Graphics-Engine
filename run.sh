#! /bin/bash
make clean
make
./image -v -e expected -d check
