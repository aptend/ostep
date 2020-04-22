
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

所以 `(2338068250 / 2001 x 10^6)s / 10^6 = 1168ns`


使用`clock_gettime`也得到了相似的结果, 单位ns，结果是`1147333720 / 10^6 = 1147ns`

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


>  如果把read(fd, NULL, 0); 换成标准输入read(0, NULL, 0) 时间会增加到1500左右，为什么呢？


## 上下文切换的时间测量
