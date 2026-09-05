#!/usr/bin/env python

"""
this is a stress testing script which runs many insmod/rmmod combinations
to check that there are no problems in loading/unloading the driver...
"""

import os
import sys


def main():
    """ main entry point """
    if len(sys.argv)!=3:
        print(f"{sys.argv[0]}: usage: module count")
        sys.exit(1)
    module=sys.argv[1]
    count=int(sys.argv[2])
    print(f"module is {module}")
    print(f"count is {count}")
    for _ in range(count):
        os.system(f"sudo insmod {module}")
        os.system(f"sudo rmmod {module}")


if __name__ == "__main__":
    main()
