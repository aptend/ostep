1. First let’s make sure you understand how the programs generally work, and
some of the key options. Study the code in vector-deadlock.c, as well
as in main-common.c and related files.
Now, run ./vector-deadlock -n 2 -l 1 -v, which instantiates two
threads (-n 2), each of which does one vector add (-l 1), and does so in
verbose mode (-v). Make sure you understand the output. How does the
output change from run to run?
2. Now add the -d flag, and change the number of loops (-l) from 1 to higher
numbers. What happens? Does the code (always) deadlock?
3. How does changing the number of threads (-n) change the outcome of the
program? Are there any values of -n that ensure no deadlock occurs?
4. Now examine the code in vector-global-order.c. First, make sure you
understand what the code is trying to do; do you understand why the code
avoids deadlock? Also, why is there a special case in this vector add()
routine when the source and destination vectors are the same?
5. Now run the code with the following flags: -t -n 2 -l 100000 -d.
How long does the code take to complete? How does the total time change
when you increase the number of loops, or the number of threads?
6. What happens if you turn on the parallelism flag (-p)? How much would
you expect performance to change when each thread is working on adding
different vectors (which is what -p enables) versus working on the same
ones?
7. Now let’s study vector-try-wait.c. First make sure you understand
the code. Is the first call to pthread mutex trylock() really needed?
Now run the code. How fast does it run compared to the global order approach? How does the number of retries, as counted by the code, change as
the number of threads increases?
8. Now let’s look at vector-avoid-hold-and-wait.c. What is the main
problem with this approach? How does its performance compare to the
other versions, when running both with -p and without it?
9. Finally, let’s look at vector-nolock.c. This version doesn’t use locks at
all; does it provide the exact same semantics as the other versions? Why or
why not?
10. Now compare its performance to the other versions, both when threads are
working on the same two vectors (no -p) and when each thread is working
on separate vectors (-p). How does this no-lock version perform?

---
```c
typedef struct __thread_arg_t
{
    int tid;                 //线程id
    int vector_add_order;    //调用顺序，0表示调用add(v[v0], v[v1]), 1表示调用add(v[v1], v[v0])
    int vector_0;            //调用发生在哪个vector上，v数组的索引v0
    int vector_1;            //调用发生在哪个vector上，v数组的索引v1
} thread_arg_t
```

1. `./vector-deadlock -n 2 -l 1 -v` 两个线程，无死锁(每个线程的加锁调用都相同)，理论上线程1和线程2的执行顺序不定，但大部分情况都是线程1先执行。`-l 50`时观察到一次上下文的切换。
2. 并不每次出现，依赖于上下文的切换，`-l 100`时才刷到一次
3. `-n`线程增多，也会增加交错的概率，n=1单线程就没得死锁撒
4. 利用指针的大小判定顺序，调用相同的数组就会产生相等的情况
5. 增加循环时间线性增加
   ```
    $ ./vector-global-order -t -n 2 -l 100000 -d
    Time: 0.03 seconds
    $ ./vector-global-order -t -n 2 -l 200000 -d
    Time: 0.06 seconds
    $ ./vector-global-order -t -n 2 -l 300000 -d
    Time: 0.08 seconds
    $ ./vector-global-order -t -n 2 -l 400000 -d
    Time: 0.11 seconds
    $ ./vector-global-order -t -n 2 -l 500000 -d
    Time: 0.14 seconds
   ```
   增加线程，因为上下文切换更频繁，非线性增加
    ```
    $ ./vector-global-order -t -n 2 -l 100000 -d
    Time: 0.03 seconds
    $ ./vector-global-order -t -n 3 -l 100000 -d
    Time: 0.06 seconds
    $ ./vector-global-order -t -n 4 -l 100000 -d
    Time: 0.14 seconds
    $ ./vector-global-order -t -n 5 -l 100000 -d
    Time: 0.17 seconds
    $ ./vector-global-order -t -n 6 -l 100000 -d
    Time: 0.22 seconds
    $ ./vector-global-order -t -n 7 -l 100000 -d
    Time: 0.26 seconds
    $ ./vector-global-order -t -n 8 -l 100000 -d
    Time: 0.31 seconds
    ```
6. 开启`p`，每个线程之间的加锁并不冲突，不必等待其他线程的执行，可以充分利用多核CPU，并发性更高，时间更少
   ```
   $ ./vector-global-order -t -n 2 -l 100000 -d -p
    Time: 0.02 seconds
    $ ./vector-global-order -t -n 3 -l 100000 -d -p
    Time: 0.03 seconds
    $ ./vector-global-order -t -n 4 -l 100000 -d -p
    Time: 0.03 seconds
    $ ./vector-global-order -t -n 5 -l 100000 -d -p
    Time: 0.03 seconds
    $ ./vector-global-order -t -n 6 -l 100000 -d -p
    Time: 0.03 seconds
    $ ./vector-global-order -t -n 7 -l 100000 -d -p
    Time: 0.03 seconds
    $ ./vector-global-order -t -n 8 -l 100000 -d -p
    Time: 0.03 seconds
   ```

7. 第一个加锁可以使用阻塞式的，加不上等就行了，没必要try。
    比全局顺序慢……挺多的。
   ```
    $ ./vector-try-wait -t -n 2 -l 100000 -d
    Retries: 547234
    Time: 0.09 seconds
    $ ./vector-try-wait -t -n 2 -l 200000 -d
    Retries: 1168988
    Time: 0.20 seconds

    $ ./vector-try-wait -t -n 3 -l 100000 -d
    Retries: 2240031
    Time: 0.19 seconds
    $ ./vector-try-wait -t -n 4 -l 100000 -d
    Retries: 5203265
    Time: 0.49 seconds
    $ ./vector-try-wait -t -n 4 -l 100000 -d
    Retries: 5203265
    Time: 2.02 seconds
   ```

8. 并发度基本么得了，连-p开启可以并发的也被全局锁限制。
   ```
   $ ./vector-avoid-hold-and-wait -t -n 4 -l 100000 -d -p
    Time: 0.08 seconds
    $ ./vector-avoid-hold-and-wait -t -n 5 -l 100000 -d -p
    Time: 0.11 seconds
    $ ./vector-avoid-hold-and-wait -t -n 6 -l 100000 -d -p
    Time: 0.15 seconds
    $ ./vector-avoid-hold-and-wait -t -n 7 -l 100000 -d -p
    Time: 0.20 seconds
   ```

9. 直接使用的硬件指令fetch-and-add。
10. ```
    $ ./vector-nolock -t -n 2 -l 100000 -d
    Time: 0.32 seconds
    $ ./vector-nolock -t -n 2 -l 200000 -d
    Time: 0.41 seconds
    $ ./vector-nolock -t -n 2 -l 300000 -d
    Time: 1.08 seconds
    $ ./vector-nolock -t -n 2 -l 400000 -d
    Time: 1.32 seconds
    $ ./vector-nolock -t -n 3 -l 100000 -d
    Time: 0.39 seconds
    $ ./vector-nolock -t -n 4 -l 100000 -d
    Time: 0.56 seconds
    $ ./vector-nolock -t -n 5 -l 100000 -d
    Time: 0.60 seconds
    $ ./vector-nolock -t -n 6 -l 100000 -d
    Time: 0.68 seconds
    $ ./vector-nolock -t -n 6 -l 100000 -d -p
    Time: 0.11 seconds
    $ ./vector-nolock -t -n 5 -l 100000 -d -p
    Time: 0.11 seconds
    $ ./vector-nolock -t -n 4 -l 100000 -d -p
    Time: 0.08 seconds
    $ ./vector-nolock -t -n 3 -l 100000 -d -p
    Time: 0.08 seconds
    $ ./vector-nolock -t -n 2 -l 100000 -d -p
    Time: 0.06 seconds
    ```
    硬件指令竟然不及加锁！光环散去了！硬件指令本身会浪费很多时间周期吧。
