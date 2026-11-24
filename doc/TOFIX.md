# TOFIX

Findings from a code scan on 2026-10-04.

## High

- `src/ser_mem.c:91-92` - on `kmalloc` failure `service_malloc` calls `ERROR(...)`, and `ERROR` in `src/kernel_helper.h:23` ends in `BUG()`, so an allocation failure crashes the kernel instead of returning NULL. Log with `pr_err` and return NULL, and have `cpp_init` in `src/driver.cc:20-22` return `-ENOMEM` when `new Driver()` yields NULL.

## Medium

- `scripts/build_kcpp.py:88-89` - the final Kbuild run hardcodes `ARCH=x86_64` and `CROSS_COMPILE=x86_64-linux-gnu-`, while the flags probe in `scripts/process_flags.py:91-97` runs `make` without them, so the C++ flags are derived from a plain `gcc` compile that differs from the one that builds the module (and the comment at `scripts/process_flags.py:88-90` claiming the module build reuses the probe object is false: the changed command line forces a rebuild). Drop the hardcoded ARCH/CROSS_COMPILE (Kbuild derives them from the kernel config), or pass the same values to both invocations.
- `scripts/test_stress_insmod_rmmod.py:22-23` - `os.system(f"sudo insmod {module}")` ignores the exit status, so a failing load/unload loop reports nothing, and interpolates an argument into a shell string. Use `subprocess.run(["sudo", "insmod", module], check=True)` (and the same for `rmmod`).
- `scripts/process_flags.py:28,59,66,74` - control flow relies on `assert`, which `python -O` strips; `find_first_matches_regexp` then returns None and line 102 crashes with an AttributeError instead of a clear message. Raise an exception or `sys.exit(...)` instead.

## Low

- `scripts/process_flags.py:51-74` - `find_ends_with` and `find_first_ends_with` are never called; delete them. `do_pass_kdir` (lines 78, 93) is a constant `True`, so the fallback branch is dead; drop it.
- `scripts/process_flags.py:113` - stale TODO about `-Wp,-MD` next to code that already removes `-Wp,-MMD`; resolve and remove the comment.
- `src/services.h` - declares about 170 `service_*` functions (pci, dma, irq, tasklet, registry, list, string, IO, ...) that no file in `src/` implements; only the memory, print, modname and empty services exist. Trim the header to what is implemented, or mark the rest as planned API.
- `src/cpp_support.cc:12` - `operator new(unsigned long)` passes its size to `service_malloc(unsigned int)` (`src/services.h:217`), silently truncating sizes above 4 GiB; widen the service signature to `unsigned long`.
- `tera.snippets/main.md.tera:24-26` - README mentions a `copy_headers` target "in makefile", but the Makefile was removed (see `rsconstruct.toml` comment); update or drop the paragraph.
- `tera.snippets/main.md.tera:40` - `http://code.google.com/p/kernelcpp/` is a dead Google Code link; point to an archive or remove it.
