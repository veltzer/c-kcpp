# Kernel build description for the kcpp module. Kbuild reads this file when
# rsconstruct runs `make -C <kernel build dir> M=<this dir> modules`. The C
# objects are built by Kbuild; the C++ objects are compiled beforehand by
# scripts/build_kcpp.py and listed here as prebuilt objects, so Kbuild links,
# runs objtool over and modposts them exactly as it does the C ones.
obj-m := kcpp.o
kcpp-objs := top.o ser_mem.o ser_print.o ser_empty.o cpp_support.o driver.o
EXTRA_CFLAGS += -Werror -I.
