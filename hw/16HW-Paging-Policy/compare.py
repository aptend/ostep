import subprocess
import re
import pprint

FILENAME = 'seq'
STAT_RE = re.compile(r"FINALSTATS hits \d+   misses \d+   hitrate (\d+.\d+)")

cache_size = list(range(5, 30, 5))


def compare_clock_bits():
    BN = 3
    bn = [[] for _ in range(BN)]

    for cache in cache_size:
        for i in range(BN):
            cmd = 'python paging-policy.py -f %s -p CLOCK -m100 -C%d -b%d -c' % (FILENAME, cache, i+1)
            print(cmd)
            bn[i].append(stat_hitrate(cmd))
    for i, b in enumerate(bn):
        print('-b%d: ' % (i+1), b)

def stat_hitrate(cmd):
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    (out, _) = p.communicate()
    match = STAT_RE.search(out)
    return match.group(1)

compare_clock_bits()
