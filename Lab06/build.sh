#!/usr/bin/env sh

# REQUIRES:
#     pkg-config
#     fmt - https://fmt.dev/

set -e

out_name=$1
out_name="${out_name%.*}"

cpp_version=-std=c++20
warnings='-Wall -Wextra -Wpedantic -Werror'
includes="$(pkg-config --cflags fmt)"
libraries="$(pkg-config --libs fmt)"
input_file=$1
target_dir=bin

mkdir -p $target_dir

printf "Building ${out_name} (*′☉.̫☉)..."

c++ $cpp_version $includes $libraries $warnings $input_file -o $target_dir/$out_name

printf ' Done! (^～^)\n'

