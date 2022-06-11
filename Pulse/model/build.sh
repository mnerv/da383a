#!/usr/bin/env sh

set -e

bin="bin"
obj="obj"

in="${1}"
out="${in%.*}"
out="${out##*/}"

cpp_version=-std=c++20
warnings="-Wall -Wextra -Wconversion -Wpedantic -Werror -Wno-missing-field-initializers"
target_dir=bin
include_dir="-I. -I../src"
library="-lgtest"
compile_flags="${cpp_version} ${warnings} ${include_dir} ${library}"

mkdir -p ${bin}
mkdir -p ${obj}

# compile
c++ ${compile_flags} ${in} -o ${bin}/${out}

