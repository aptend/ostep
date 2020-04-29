"""
usage:

valgrind --tool=lackey --trace-mem=yes --log-fd=1 <program> | python convert.py > addr
"""

import re

ADDR_RE = re.compile(r'([a-f0-9]+),\d{1,2}\s$')

import sys

for line in sys.stdin:
    match = ADDR_RE.search(line)
    if match:
        vpn = match.group(1)[:-3]
        try:
            addr = int(vpn, 16)
        except ValueError:
            print(repr(line))
            raise
        print(addr)

