# Inlines and relocations in C++ kernel modules

Notes on a problem we hit writing kernel modules in C++: modules that built
fine refused to load because of relocations the kernel does not understand.

## The symptom

When gcc and the linker meet inline functions that are "big" (we do not know
exactly what that means) the resulting module fails to load with:

```text
Unknown relocation: 0
```

## Looking at the relocations

To see the relocations in an object file or a module use:

```bash
objdump -r [object file or .ko file]
```

Relocations come in three flavours:

* `RX`
* `RX 386`
* `NONE`

Here is an example:

```text
$ objdump -r obj/modules/KFcb.ko | c++filt | grep NONE
000009d8 R_386_NONE        *ABS*
000009ec R_386_NONE        *ABS*
00002b08 R_386_NONE        *ABS*
00002b1c R_386_NONE        *ABS*
00003698 R_386_NONE        *ABS*
.... more here ....
```

The Linux kernel knows how to handle only the first two kinds. The user space
dynamic linker probably knows how to handle all of them.

## What we know

* We do not know how to stop the compiler from emitting these relocations
  (whether there is some magic flag to turn them off). This needs to be
  investigated.
* We *did* prove that turning the offending functions from inline into
  non-inline makes the problem go away.

## Addition (31 October 2009)

An attempt to compile with `-fpic` / `-fPIC` did not make the problem go
away. The error became:

```text
[28373.931835] KFcb: Unknown symbol _GLOBAL_OFFSET_TABLE_
```

`-fPIC` relies on the dynamic linker to provide this symbol and the kernel's
module loader does not supply that service. Other attempts (`-mcmodel`,
which does not apply) did not work either.

**Success!** Adding `-fno-exceptions` makes the problem go away. The compiler
was generating exception handling code even though the code had nothing to
do with exceptions, and the `NONE` relocations came from that.

Mark Veltzer
