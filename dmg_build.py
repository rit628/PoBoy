#!/usr/bin/env python3

import argparse
from subprocess import run
from multiprocessing import cpu_count
from pathlib import Path
from shutil import copy2 as copy

def build(args):
    build_mode = f"-DCMAKE_BUILD_TYPE={args.mode}"
    compile_sst = f"-DCOMPILE_SST={"ON" if args.sst else "OFF"}"
    toolchain_file = f"-DCMAKE_TOOLCHAIN_FILE=./cmake/{args.target_platform}.cmake" if args.target_platform != "local" else ""
    output_directory = Path("build", args.target_platform)
    cmake_args = list(filter(None, [ "-DCMAKE_C_COMPILER=clang"
                                   , "-DCMAKE_CXX_COMPILER=clang++"
                                   , "-DCMAKE_LINKER_TYPE=LLD"
                                   , "-G Ninja"
                                   , "-DCMAKE_COLOR_DIAGNOSTICS=ON"
                                   , build_mode
                                   , toolchain_file
                                   , compile_sst
                                   ]))

    run(["cmake", "-S", ".", "-B", output_directory, *cmake_args])
    run(["cmake", "--build", output_directory, f"-j{args.cores}", "-t", *args.targets])
    copy(output_directory / Path("compile_commands.json"), Path("build", "compile_commands.json"))


parser = argparse.ArgumentParser(description="PoBoy build script", formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument("targets"
                   , help="build targets"
                   , nargs="*"
                   , default=["all"])

parser.add_argument("-m", "--mode"
                   , help="set build optimization mode"
                   , choices=["debug", "release", "minsizerel", "relwithdebinfo"]
                   , default="debug")

parser.add_argument("-t", "--target-platform"
                   , help="set build platform target"
                   , choices=["local", "windows"]
                   , default="local")

parser.add_argument("--cores"
                   , help="set number of cores to use during compilation"
                   , type=int
                   , default=cpu_count())

parser.add_argument("--sst"
                   , help="compile sst tests"
                   , action="store_true")

if __name__ == "__main__":
    args = parser.parse_args()
    build(args)