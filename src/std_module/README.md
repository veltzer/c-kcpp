# std_module

A minimal "hello, world" kernel module, built by Kbuild from the `Kbuild`
file next to it. It exists so that `scripts/process_flags.py` can build it
verbosely and read off the exact C compiler flags the kernel uses, which it
then turns into the flags for the C++ parts of the kcpp module.

It is built into `out/std_module`, never in place.
