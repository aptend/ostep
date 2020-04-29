import re
import sys
import subprocess

FILENAME = 'addr'
STAT_RE = re.compile(r"FINALSTATS hits \d+   misses \d+   hitrate (\d+.\d+)")

def build_cmd(filename, policy, cache_size):
    return 'python paging-policy.py -f %s -p %s -C%d -c -N' % (filename, policy, cache_size)


def compare(filename, sizes, policies):
    stat = [[] for _ in range(len(sizes))]
    for size in sizes:
        for i, policy in enumerate(policies):
            stat[i].append(stat_hitrate(build_cmd(filename, policy, size)))
    print('cache size', sizes)
    for p, s in zip(policies, stat):
        print(p, s)

def compare_rand_lru(filename):
    cache_size = list(range(10, 20))
    compare_set = ['LRU', 'RAND']
    compare(filename, cache_size, compare_set)


def compare_opt_rand_lru(filename):
    cache_size = list(range(5, 100, 10))
    compare_set = ['OPT', 'LRU', 'RAND']
    compare(filename, cache_size, compare_set)

def compare_clock_bits(filename):
    cache_size = list(range(1, 40, 3))
    compare_set = ['CLOCK -b1', 'CLOCK -b2', 'CLOCK -b3']
    compare(filename, cache_size, compare_set)

def stat_hitrate(cmd):
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    (out, _) = p.communicate()
    match = STAT_RE.search(out)
    return match.group(1)

tasks = [
    compare_opt_rand_lru,
    compare_clock_bits,
    compare_rand_lru,
]

if len(sys.argv) < 2:
    print('specify a compare task')
    for i, f in enumerate(tasks):
        print("{}: {}".format(i, f.__name__))
    sys.exit(0)
filename = sys.argv[2] if len(sys.argv) == 3 else 'seq'
tasks[int(sys.argv[1])].__call__(filename)
