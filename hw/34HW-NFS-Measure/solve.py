#! usr/bin/python3

import os
import sys
from datetime import datetime
from collections import Counter
import subprocess

CTime = 0
CMahineA = 1
CMahineB = 2
CMsgType = 4
CTxId = 5
COpNum = 6
COpName = 7

INPUT = 'nfs-example.txt'


def p1():
    print('P1\n--------------------')

    with open(INPUT) as f:
        _start = datetime.fromtimestamp(float(f.readline().split()[CTime]))

        f.seek(os.fstat(f.fileno()).st_size - 300)
        last = f.read().strip().split('\n')[-1].split()[CTime]
        _end = datetime.fromtimestamp(float(last))

        _delta = _end - _start
        print(f'start time\t {_start}\n'
              f'end time\t {_end}\n'
              f'delta time\t {_delta}')


def p2():
    print('P2\n--------------------')
    with open(INPUT) as f:
        c = Counter([line.split(' ', 8)[COpName] for line in f])
    import pprint
    pprint.pprint(c.most_common())


if len(sys.argv) != 2:
    print("specify a problem to solve")
    sys.exit(0)

locals()['p'+str(sys.argv[1])].__call__()
