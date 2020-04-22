
## 时间戳计时

ref:

[blog: 多核环境下rdtsc不适用-2010](https://blog.csdn.net/solstice/article/details/5196544)  
[blog: 再论TimeStampCounter-2012](https://www.cnblogs.com/ralphjzhang/archive/2012/01/09/2317463.html)

[zhihu: 时间测量比较-2015](https://www.zhihu.com/question/28559933/answer/41346116)

`rdtsc`是直接数CPU经过了多少个时间周期, `ReaD TimeStamp Counter`

根据第二篇文章，主频变化、多核技术器基准不同的问题都解决了，乱序执行的问题虽然还在，但是可以用`rdtscp`来同步。


这里还是用`rdtsc`来测量，结果如下
```
1000000 times read: 3318511864
1000000 times read: 3276549532
1000000 times read: 3270953560
1000000 times read: 3277160878
1000000 times read: 3256661022
1000000 times read: 3268336348
1000000 times read: 3295690668
1000000 times read: 3274807988
1000000 times read: 3267876860
1000000 times read: 3261003352
```
```
$ cat /proc/cpuinfo
processor       : 0
vendor_id       : GenuineIntel
cpu family      : 6
model           : 60
model name      : Intel(R) Core(TM) i3-4160 CPU @ 3.60GHz
stepping        : 3
microcode       : 0xffffffff
cpu MHz         : 3600.000
cache size      : 256 KB
physical id     : 0
siblings        : 4
core id         : 0
cpu cores       : 2
apicid          : 0
initial apicid  : 0
fpu             : yes
fpu_exception   : yes
cpuid level     : 6
wp              : yes
```

所以 `3256661022 / 10^6 / 3600 x 10 ^ 6 * 10^6 ms = 0.9ms`
