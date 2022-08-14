#!/usr/bin/env sh

# REQUIRES:
#     pkg-config
#     fmt - https://fmt.dev/

set -e

ESCAPE_CODE='\033'
RED='\033[0;31m'
NC='\033[0m'      # No Color
if [ $# -eq 0 ]; then
    printf "${RED}No input file${NC} (っ- ‸ – ς)\n"
    printf "\n"
    printf "Usage: ${0} {filename}.cpp\n"
    exit 1
fi

bin=bin

cpp_version=-std=c++20
warnings='-Wall -Wextra -Wpedantic -Werror'
includes="$(pkg-config --cflags fmt)"
libraries="$(pkg-config --libs fmt)"

input=$1
out="${input%.*}"
target=${bin}/${out}

mkdir -p $bin

printf "Building ${out} (*′☉.̫☉)..."

c++ $cpp_version $includes $libraries $warnings $input -o $target

printf ' Done! (^～^)\n'

