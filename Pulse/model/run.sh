#!/usr/bin/env sh

in=$1
out="${in%.*}"
out=${out##*/}

./build.sh $in
./bin/$out "${@:2}"

