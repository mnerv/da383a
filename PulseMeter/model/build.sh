#!/usr/bin/env sh

set -e

out_name=$1
out_name="${out_name%.*}"

cpp_version=-std=c++20
warnings="-Wall -Wextra -Wpedantic -Werror"
input_file=$1
target_dir=bin
include_dir="-I./ -I../src"
compile_flags="${cpp_version} ${warnings} ${include_dir}"

mkdir -p $target_dir

echo "Building ${out_name}..."

c++ $compile_flags $input_file -o $target_dir/$out_name

green=$"\u001b[32m"
reset=$"\u001b[0m"

echo "Done!\n"

