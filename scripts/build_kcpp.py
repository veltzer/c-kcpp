#!/usr/bin/env python

""" Build the kcpp kernel module, replacing the old Makefile.

Sources live in src/ (described by src/Kbuild). The build runs in a
temporary directory: src/ is copied there, everything is compiled and
linked in place, and only the results (kcpp.ko and the flags.cfg it was
built with) are copied to out/. The source tree is never written to, and
none of Kbuild's by-products end up in the repository - neither the objects
and .cmd files of an in-tree build nor the source symlink and Makefile that
its MO= output mode creates.

The module mixes C (built by Kbuild) with C++ that Kbuild cannot compile.
The sequence, all inside the temporary directory, is:
  1. scripts/process_flags.py has Kbuild compile one of the module's own C
     objects verbosely and derives C++ compiler flags from the gcc command
     line it prints into flags.cfg
  2. every .cc file is compiled with g++ using those flags
  3. Kbuild builds the C objects and links the module, taking the C++
     objects as prebuilt members listed in Kbuild

Handing the C++ objects to Kbuild (rather than relinking the module by hand
afterwards, as the Makefile did) matters on current kernels: Kbuild's link
merges the per-function __patchable_function_entries sections through the
kernel's module linker script (left separate, the kernel refuses the module
with EEXIST when it creates their sysfs entries), and Kbuild runs objtool
over the linked object so the return thunks in the C++ code get their
.return_sites annotations (without them the kernel warns "Unpatched return
thunk in use" at load). The only thing Kbuild needs for a prebuilt object is
a .<obj>.cmd file next to it, which modpost reads to learn its source file.

The kernel build directory is the KDIR environment variable if set (as the
Makefile's KDIR could be), else the running kernel's headers if installed,
else the newest kernel headers present under /lib/modules (what the
linux-headers-generic package provides on a CI runner whose own kernel has
no headers package). """

import glob
import os
import shutil
import subprocess
import sys
import tempfile

NAME = "kcpp"
SRC = "src"
OUT = "out"
FLAGS = "flags.cfg"
# the C object whose compile command line the C++ flags are derived from
FLAGS_PROBE_OBJECT = "top.o"
RESULTS = [f"{NAME}.ko", FLAGS]


def run(args, cwd=None):
    """ run a command, aborting the build on failure """
    ret = subprocess.call(args, cwd=cwd)
    if ret != 0:
        sys.exit(ret)


def kernel_build_dir():
    """ pick the kernel headers to build against """
    running = f"/lib/modules/{os.uname().release}/build"
    if os.path.isdir(running):
        return running
    candidates = sorted(d for d in glob.glob("/lib/modules/*/build") if os.path.isdir(d))
    if not candidates:
        sys.exit("no kernel headers found under /lib/modules; install linux-headers-generic")
    return candidates[-1]


def build(kdir, build_dir):
    """ build the module inside build_dir, which holds a copy of src/ """
    scripts = os.path.dirname(os.path.abspath(__file__))
    run([os.path.join(scripts, "process_flags.py"), kdir, build_dir, FLAGS_PROBE_OBJECT,
         os.path.join(build_dir, FLAGS)])
    with open(os.path.join(build_dir, FLAGS), encoding="utf-8") as handle:
        flags = handle.read().split()
    for source in sorted(glob.glob(os.path.join(build_dir, "*.cc"))):
        source = os.path.basename(source)
        obj = source[:-3] + ".o"
        cmd = ["g++"] + flags + ["-Wall", "-Werror", "-I.", "-c", "-o", obj, source]
        run(cmd, cwd=build_dir)
        # what Kbuild records for the objects it compiles itself; modpost
        # reads it to find the object's source file
        with open(os.path.join(build_dir, f".{obj}.cmd"), "w", encoding="utf-8") as handle:
            handle.write(f"savedcmd_{obj} := {' '.join(cmd)}\n")
    run(["make", "-C", kdir, f"M={build_dir}", "ARCH=x86_64",
         "CROSS_COMPILE=x86_64-linux-gnu-", "modules"])


def main():
    """ main entry point """
    kdir = os.environ.get("KDIR") or kernel_build_dir()
    with tempfile.TemporaryDirectory(prefix=f"{NAME}-build-") as build_dir:
        shutil.copytree(SRC, build_dir, dirs_exist_ok=True)
        build(kdir, build_dir)
        os.makedirs(OUT, exist_ok=True)
        for result in RESULTS:
            shutil.copy2(os.path.join(build_dir, result), os.path.join(OUT, result))


if __name__ == "__main__":
    main()
