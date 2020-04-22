
## 系统调用的时间测量

ref:

[blog: 多核环境下rdtsc不适用-2010](https://blog.csdn.net/solstice/article/details/5196544)  
[blog: 再论TimeStampCounter-2012](https://www.cnblogs.com/ralphjzhang/archive/2012/01/09/2317463.html)

[zhihu: 时间测量比较-2015](https://www.zhihu.com/question/28559933/answer/41346116)

`rdtsc`是直接数CPU经过了多少个时间周期, `ReaD TimeStamp Counter`

根据第二篇文章，主频变化、多核技术器基准不同的问题都解决了，乱序执行的问题虽然还在，但是可以用`rdtscp`来同步。


这里还是用`rdtsc`来测量，结果如下
```
$ ./a.out
1000000 times read cost: 2282567837
1000000 times read cost: 2278176314
1000000 times read cost: 2325685633
1000000 times read cost: 2273940101
1000000 times read cost: 2269210048
1000000 times read cost: 2301289353
1000000 times read cost: 2352414939
1000000 times read cost: 2518596276
1000000 times read cost: 2359600308
1000000 times read cost: 2419201691
average: 2338068250
```
```
$ cat /proc/cpuinfo 
processor       : 0
vendor_id       : GenuineIntel
cpu family      : 6
model           : 142
model name      : Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz
stepping        : 10
microcode       : 0xffffffff
cpu MHz         : 2001.000
cache size      : 256 KB
physical id     : 0
siblings        : 8
core id         : 0
cpu cores       : 4
apicid          : 0
initial apicid  : 0
fpu             : yes
fpu_exception   : yes
cpuid level     : 6
wp              : yess
```

所以 `(2338068250 / 2001 x 10^6)s / 10^6 = 1168纳秒 = 1.16微秒`


使用`clock_gettime`也得到了相似的结果, 单位ns，结果是`1147333720 / 10^6 = 1147纳秒 = 1.14微秒`

```
$ ./a.out
1000000 times read cost: 1143889000
1000000 times read cost: 1147410300
1000000 times read cost: 1160507300
1000000 times read cost: 1144955400
1000000 times read cost: 1148255500
1000000 times read cost: 1144968800
1000000 times read cost: 1143389500
1000000 times read cost: 1148820800
1000000 times read cost: 1148736700
1000000 times read cost: 1142403900
average: 1147333720
```

> write(fd, NULL, 0) 少一些，900ns左右

> 如果把read(fd, NULL, 0); 换成标准输入read(0, NULL, 0) 时间会增加到1500左右，为什么呢？


## 上下文切换的时间测量

用提示的双管道的方法进行

> 如果两组write/read真的完全乱序进行，那么一轮4次系统调用固定，但是上下文切换完全可以达到三次啊，这怎么算？


10^6轮，平均每轮4474ns，4.4微秒，如果减去之前的系统调用测量，上下文切换都没剩下了呀？不懂


lmbench的结果如下，4~5微妙
```txt
Context switching - times in microseconds - smaller is better
-------------------------------------------------------------------------
Host                 OS  2p/0K 2p/16K 2p/64K 8p/16K 8p/64K 16p/16K 16p/64K
                         ctxsw  ctxsw  ctxsw ctxsw  ctxsw   ctxsw   ctxsw
--------- ------------- ------ ------ ------ ------ ------ ------- -------
DESKTOP-E Linux 4.4.0-1 3.8800 4.5000 4.8300 5.0500 5.3800 5.29000 5.61000
```
