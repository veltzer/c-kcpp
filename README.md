# *c-kcpp* project by Mark Veltzer

description: C++ framework for the linux kernel

project website: https://veltzer.github.io/c-kcpp

author: Mark Veltzer

version: 0.0.1

![Code style: black](https://img.shields.io/badge/code%20style-black-000000.svg)

## github

![License](https://img.shields.io/github/license/veltzer/c-kcpp)

## build

![build](https://github.com/veltzer/c-kcpp/workflows/build/badge.svg)
POC for writing C++ modules for the Linux kernel
================================================

## The problem:
C++ has lots of issues which are not dealt with in the kernel.
* Problems in linking.
* Exception handling that needs to be turned off.
* Memory management functions (like new and delete) which need to be implemented.
* The kernel headers are not suitable for inclusion for a C++ compiler
since they conflict with many C++ constructs ('class' for instance and more).
* The kernel build system does not compile C++ correctly, or even at all.

## The idea:
* separate the "top part" - the one that is getting called by Linux, like driver
entry and exit points, interrupt functions, tasklets, whatever and handle them in a C layer.
expose APIs for all of these facilities and other low level facilities (printk, hardware access
and more) via C functions.
* write a top part that only accesses these services by well controlled C functions which do
not include the kernel headers and are C++ friendly. In the top part write a framework for
device driver writing based on OO concepts (inherit from Device and implement the relevant
methods).

## Future ideas:
* Add ability to include header files directly in C++ code.
Progress: have 'copy_headers' target in makefile
which also applies patch but headers are still hard to
work with...
This is to be developed as a patch to the linux kernel (big project - watch out!
may never happen...)
* add a framework for writing char drivers

## Layout:
* `src/` - the module sources: C, C++, headers and the `Kbuild` file that
describes the module to the kernel build system.
* `src/std_module/` - a minimal sample module, built only to read off the
C flags the kernel uses so they can be adapted for the C++ parts.
* `scripts/` - the build driver (`build_kcpp.py`, run by rsconstruct) and helpers.
* `doc/` - notes on the problems met along the way, in markdown.
* `out/` - the build results: `kcpp.ko` and the `flags.cfg` its C++ parts were
compiled with. The build itself runs in a temporary directory, so nothing else
is produced. Not checked in; `rm -rf out` is a full clean.

## Related projects:
* http://code.google.com/p/kernelcpp/
They do not have a buffer layer but instead do "extern C" before kernel headers.
Try to see if this works for a big example.
They have all kinds of other ideas about constructors for global object and so on.
* http://www.drdobbs.com/cpp/c-exceptions-the-linux-kernel/229100146

## contact me

[mailto](mailto:mark.veltzer@gmail.com)
![gitter](https://img.shields.io/gitter/room/veltzer/mark.veltzer)
![discord](https://img.shields.io/discord/719336281624281119)
![discord](https://img.shields.io/discord/719336282194444302)

Mark Veltzer, Copyright © 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019, 2020, 2021, 2022, 2023, 2024, 2025, 2026
