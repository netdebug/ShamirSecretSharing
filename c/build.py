#!/usr/bin/env python

import os
import sys
import shutil
import subprocess

base_dir = os.path.dirname(os.path.abspath(__file__))

def main():
    try:
        build_dir = os.path.join(base_dir, "build")
        if os.path.isdir(build_dir):
            shutil.rmtree(build_dir)
        os.makedirs(build_dir)
        os.chdir(build_dir)

        if os.name == "posix":
            generator = 'Unix Makefiles'
        elif os.name == "nt":
            generator = 'NMake Makefiles'

        out = subprocess.check_output("cmake -G \"" + generator + "\" ..", shell=True, stderr=subprocess.STDOUT)
        print(str(out))
        out = subprocess.check_output(["cmake", "--build", ".", "--target", "all"], stderr=subprocess.STDOUT)
        print(str(out))
        out = subprocess.check_output(["ctest", "-T", "Test"], stderr=subprocess.STDOUT)
        print(str(out))
    except Exception as e:
        sys.stderr.write(str(e) + "\n")
        sys.exit(1)
    try:
        subprocess.check_output(["ctest", "-T", "memcheck"], stderr=subprocess.STDOUT)
    except Exception as e:
        sys.stderr.write(str(e) + "\n")
        sys.stderr.write("Memory checking is not possible...\n")
        sys.exit(0)

if __name__ == "__main__":
    main()
    sys.exit(0)
