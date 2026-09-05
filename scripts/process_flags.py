#!/usr/bin/env python3

"""
This is a script that processes flags from linux kernel compilation and
produces the flags for c++ compilation
"""

import os
import re
import subprocess
import sys

# General guide to compiler flags (this is true in MOST cases, not in ALL
# cases, check each flag...)
# flags starting with -f or -m: affect code generation
# flags starting with -W: produce more warnings, dont affect code generation


def remove_begin_with(elems, search):
    """ removed all elemnets from a python list that begin with ... """
    found=False
    res=[]
    for elem in elems:
        if elem.startswith(search):
            found=True
        else:
            res.append(elem)
    assert found, f"could not find stuff begining with [{search}]"
    return res

def remove_two_in_a_row(elems, search):
    """ remove two elements in a row from a list """
    res=[]
    found=False
    for elem in elems:
        if elem==search:
            found=True
        else:
            if found:
                found=False
            else:
                res.append(elem)
    return res

def remove_if_exists(elems, search):
    """ remove all elements from a list that contain a substring """
    if search in elems:
        elems.remove(search)
    return elems

def find_ends_with(elems, ending):
    """ return a single element in a list that has a certain ending """
    found = None
    found_count=0
    for elem in elems:
        if elem.endswith(ending):
            found_count+=1
            found=elem
    assert found_count==1, f"found too many or too little [{found_count}]"
    return found

def find_first_matches_regexp(elems, regexp):
    for elem in elems:
        if regexp.match(elem):
            return elem
    assert False, "could not find element by regexp"
    return None


def find_first_ends_with(elems, ending):
    for elem in elems:
        if elem.endswith(ending):
            return elem
    assert False, "element not found"

def main():
    """ main entry point """
    do_pass_kdir=True
    do_clean=True
    if len(sys.argv)!=4:
        print(f"{sys.argv[0]}: usage: {sys.argv[0]} kdir std_module_dir outfile")
        sys.exit(1)
    kdir=sys.argv[1]
    std_module=os.path.abspath(sys.argv[2])
    outfile=sys.argv[3]

    # build the sample module (described by the Kbuild file in std_module_dir,
    # a scratch copy of src/std_module that scripts/build_kcpp.py makes in its
    # temporary build directory) verbosely, so the kernel prints the exact
    # gcc command line it uses
    args=[
        "make",
        "-C", kdir if do_pass_kdir else f"/lib/modules/{os.uname().release}/build",
        f"M={std_module}",
        "V=1",
        "W=1",
    ]
    if do_clean:
        subprocess.check_output(args+["clean"])
    output=subprocess.check_output(args+["modules"]).decode()
    # split into lines and find the line that ends with "main.c"
    lines=output.split("\n")
    # old code
    # line=find_first_ends_with(lines, "main.c")
    # line=line.strip()
    # new code
    regexp = re.compile("^ *gcc")
    line=find_first_matches_regexp(lines, regexp)
    line = line.split(";")[0].strip()
    flag_list=line.split()
    # remove the last component (which is the source file name, see above)
    flag_list=flag_list[:-1]
    # header file stuff, does not influence code generation
    # C++ code does not reference any header files of the kernel,
    # operating system, or compiler...
    flag_list = remove_begin_with(flag_list, "-I")
    # remove macros of the kernel (which we dont use in the cpp layer)
    # flag_list = remove_begin_with(flag_list, "-D")
    # remove the -Wp, MD
    # TODO - this was remarked because the -Wp -MD flags were not found. Is this ok?
    flag_list=remove_begin_with(flag_list, "-Wp,-MMD")
    # remove -include and -isystem (which we dont use in the cpp layer)
    flag_list = remove_two_in_a_row(flag_list, "-isystem")
    flag_list = remove_two_in_a_row(flag_list, "-include")
    flag_list = remove_two_in_a_row(flag_list, "-o")
    # if the kernel was compiled with debug and profiling then we dont
    # need it
    # flag_list.remove("-pg")
    flag_list.remove("-c")
    remove_flags = {
        "-nostdinc", # header file stuff, does not influence code generation
        "gcc-14",
        "gcc-13",
        "gcc-12",
        "gcc-11",
        "gcc",
        "-Werror=strict-prototypes",
        "-Werror=implicit-function-declaration",
        "-Werror=implicit-int",
        "-Werror=designated-init",
        "-Werror=incompatible-pointer-types",
        "-Wdeclaration-after-statement",
        "-Wno-pointer-sign",
        "-Wmissing-prototypes",
        "-Wold-style-definition",
        "-std=gnu90",
        "-std=gnu89",
        "-std=gnu11",
    }
    for remove_flag in remove_flags:
        remove_if_exists(flag_list, remove_flag)
    # l.remove("-g")
    # remove flags which are not valid for C++...
    # cc1plus: warning: command line option "-Wstrict-prototypes" is valid for
    # C/ObjC but not for C++ [enabled by default]
    # cc1plus: warning: command line option "-Wdeclaration-after-statement" is valid for
    # C/ObjC but not for C++ [enabled by default]
    # cc1plus: warning: command line option "-Wno-pointer-sign" is valid for
    # C/ObjC but not for C++ [enabled by default]
    with open(outfile, "w", encoding="UTF8") as stream:
        stream.write(" ".join(flag_list))

if __name__ == "__main__":
    main()
