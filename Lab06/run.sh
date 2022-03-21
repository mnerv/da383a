#!/usr/bin/env sh

source ./build.sh

target=$1
target="${target%.*}"
args="${@:2}"

echo $target

./bin/$target $args

