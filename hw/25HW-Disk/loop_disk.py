import mock

from disk import Disk
from collections import namedtuple
args = namedtuple('Args', 'seed')


for seed in range(3):
    options = args(seed)
    print('----------------seed {}------------------'.format(options.seed))
    with mock.patch.dict(Disk.__init__.__globals__, options=options):
        for w in range(1, 1002, 200):
            d = Disk(addr='-1', addrDesc='1000,-1,0', lateAddr='-1',
                     lateAddrDesc='0,-1,0', policy='SATF',
                     seekSpeed=1.0, rotateSpeed=1.0,
                     skew=0, window=w,
                     compute=False, graphics=False, zoning='30,30,30')
            d.Go()
            print("win {}, total:{}".format(w, d.timer))
