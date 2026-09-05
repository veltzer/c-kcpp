#!/usr/bin/env python

""" Build the kcpp kernel module, replacing the old Makefile.

The module mixes C (built by Kbuild from the Kbuild file next to this
script's parent) with C++ that Kbuild cannot compile. The sequence is:
  1. scripts/process_flags.py derives C++ compiler flags from the kernel's
     own C flags into flags.cfg
  2. every .cc file is compiled with g++ using those flags
  3. Kbuild builds the C objects and links the module, taking the C++
     objects as prebuilt members listed in its Kbuild file

Handing the C++ objects to Kbuild (rather than relinking the module by hand
afterwards, as the Makefile did) matters on current kernels: Kbuild's link
merges the per-function __patchable_function_entries sections through the
kernel's module linker script (left separate, the kernel refuses the module
with EEXIST when it creates their sysfs entries), and Kbuild runs objtool
over the linked object so the return thunks in the C++ code get their
.return_sites annotations (without them the kernel warns "Unpatched return
thunk in use" at load). The only thing Kbuild needs for a prebuilt object is
a .<obj>.cmd file, which modpost reads to learn its source file.

The kernel build directory is the KDIR environment variable if set (as the
Makefile's KDIR could be), else the running kernel's headers if installed,
else the newest kernel headers present under /lib/modules (what the
linux-headers-generic package provides on a CI runner whose own kernel has
no headers package). """

import glob
import os
import subprocess
import sys

FLAGS = "flags.cfg"


def run(args):
    """ run a command, aborting the build on failure """
    ret = subprocess.call(args)
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


def main():
    """ main entry point """
    kdir = os.environ.get("KDIR") or kernel_build_dir()
    run(["scripts/process_flags.py", kdir, FLAGS])
    with open(FLAGS, encoding="utf-8") as handle:
        flags = handle.read().split()
    for source in sorted(glob.glob("*.cc")):
        obj = source[:-3] + ".o"
        cmd = ["g++"] + flags + ["-Wall", "-Werror", "-c", "-o", obj, source]
        run(cmd)
        # what Kbuild records for the objects it compiles itself; modpost
        # reads it to find the object's source file
        with open(f".{obj}.cmd", "w", encoding="utf-8") as handle:
            handle.write(f"savedcmd_{obj} := {' '.join(cmd)}\n")
    run(["make", "-C", kdir, f"M={os.getcwd()}", "ARCH=x86_64",
         "CROSS_COMPILE=x86_64-linux-gnu-", "modules"])
    # Kbuild's object list for the link; the fleet .gitignore does not cover
    # *.mod, so it must not be left in the tree
    if os.path.exists("kcpp.mod"):
        os.remove("kcpp.mod")


if __name__ == "__main__":
    main()
