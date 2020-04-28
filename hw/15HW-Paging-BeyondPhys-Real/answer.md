1. First, open two separate terminal connections to the same machine, so that
you can easily run something in one window and the other.
Now, in one window, run vmstat 1, which shows statistics about machine
usage every second. Read the man page, the associated README, and any
other information you need so that you can understand its output. Leave
this window running vmstat for the rest of the exercises below.
Now, we will run the program mem.c but with very little memory usage.
This can be accomplished by typing ./mem 1 (which uses only 1 MB of
memory). How do the CPU usage statistics change when running mem? Do
the numbers in the user time column make sense? How does this change
when running more than one instance of mem at once?
2. Let’s now start looking at some of the memory statistics while running mem.
We’ll focus on two columns: swpd (the amount of virtual memory used) and
free (the amount of idle memory). Run ./mem 1024 (which allocates 1024
MB) and watch how these values change. Then kill the running program
(by typing control-c) and watch again how the values change. What do you
notice about the values? In particular, how does the free column change
when the program exits? Does the amount of free memory increase by the
expected amount when mem exits?
3. We’ll next look at the swap columns (si and so), which indicate how much
swapping is taking place to and from the disk. Of course, to activate these,
you’ll need to run mem with large amounts of memory. First, examine how
much free memory is on your Linux system (for example, by typing cat
/proc/meminfo; type man proc for details on the /proc file system and
the types of information you can find there). One of the first entries in
/proc/meminfo is the total amount of memory in your system. Let’s assume it’s something like 8 GB of memory; if so, start by running mem 4000
(about 4 GB) and watching the swap in/out columns. Do they ever give
non-zero values? Then, try with 5000, 6000, etc. What happens to these
values as the program enters the second loop (and beyond), as compared to
the first loop? How much data (total) are swapped in and out during the
second, third, and subsequent loops? (do the numbers make sense?)
4. Do the same experiments as above, but now watch the other statistics (such
as CPU utilization, and block I/O statistics). How do they change when
mem is running?
5. Now let’s examine performance. Pick an input for mem that comfortably
fits in memory (say 4000 if the amount of memory on the system is 8 GB).
How long does loop 0 take (and subsequent loops 1, 2, etc.)? Now pick a size
comfortably beyond the size of memory (say 12000 again assuming 8 GB of
memory). How long do the loops take here? How do the bandwidth numbers compare? How different is performance when constantly swapping
versus fitting everything comfortably in memory? Can you make a graph,
with the size of memory used by mem on the x-axis, and the bandwidth of
accessing said memory on the y-axis? Finally, how does the performance of
the first loop compare to that of subsequent loops, for both the case where
everything fits in memory and where it doesn’t?
6. Swap space isn’t infinite. You can use the tool swapon with the -s flag to
see how much swap space is available. What happens if you try to run mem
with increasingly large values, beyond what seems to be available in swap?
At what point does the memory allocation fail?
7. Finally, if you’re advanced, you can configure your system to use different
swap devices using swapon and swapoff. Read the man pages for details.
If you have access to different hardware, see how the performance of swapping changes when swapping to a classic hard drive, a flash-based SSD, and
even a RAID array. How much can swapping performance be improved via
newer devices? How close can you get to in-memory performance?

---

![J4tJUO.png](https://s1.ax1x.com/2020/04/28/J4tJUO.png)


1. `vmstat 1 -w -S M` 每秒刷新，宽距打印，内存单位MB。 `us`是运行用户代码的时间，在4核机器上，mem运行时固定为25%，单核机器上100%

> -S M只会影响到Memory分组的单位  
> bo bi 的单位是block/s，一个block是1KB
> so si 的单位KB/s,

2. 在两个机器上测试过，一个机器free很充足，所以free栏的确随mem启动减少，mem退出而增加，变化量就是mem的参数。另一个机器free不足，会从cache一栏中减少内存供mem使用，退出后，free会变大


3. 找了一个内存3.7G，swap为3.9G的单核机器，cache 1229， free 1644， mem 2000启动时，cache 779， free 173， 这是会发生so，总共换出了166MB(224->390)。只会在第一次循环时发生换出，后续的循环so都为0。具体的si\so数字，感觉和单位如文档说的KB/s有出入啊，就是每次so就等于每次swpd的增量，那单位就是MB？
> swapon -s = cat /proc/swaps, 单位是K  
> Filename   |   Type   |   Size   |   Used   |   Priority 
> --|--|--|--|--
> /dev/dm-1 | partition | 4063228 | 399404 | -1

4. CPU的结果，在还可以全部放入内存时，没有IO，还是us一项100%占满。IO在有so、si的时候发生，bo bi的单位还是KB，每秒采集一次，bo等于so * 1024

5. 分为两种情况，能完全放入物理内存时(./mem 3000)，loop0，因为要重新添加TLB缓存，所以比较慢，bandwidth为`900~1200MB/s`，loop1开始，TLB的命中率提高，bandwidth稳定在`2400MB/s`左右。不能完全放入内存时(./mem 3500)，每次都伴随着si和so，vmstat刷新周期内，CPU并不是都在执行代码，us wa的比例从1-9开，到5-5开都有，所以IO速度开始拖慢程序的运行，此时的loop的bandwidth的波动比较大，`580, 322, 264, 429, 643, 856, 1364, 159, 525...`，但是整体已经比之前的`2400`下降了很多。



6. 我这里6500时就直接内存分配失败，剩下的应该是操作系统和其他进程的保留。6000MB的首轮，so显示系统以小于10MB/s的速度在往外送页，首轮的bandwidth是`20.18MB/s`，第二轮`33.02MB/s`，第三轮`26.73MB/s`，第四轮`25.57MB/s`。这个时候，vmstat的刷新周期内，占比基本没什么变化，us占比1~3%， sys占比8~20%，剩余大部分时间都是wa，等待IO，占比88~100%，感觉每个页都要经过IO，可以看到内存和机械硬盘的速度差别，100倍左右的差距。

    ```
    mem 3000 | 1342, 2408, 2411, 2405, 2432, ...
    mem 3500 | 580, 322, 264, 429, 643, 856, 1364, 159, 525, ...
    mem 4000 | 350, 28.76, 33.97, 35.57...
    mem 6000 | 20.18, 33.02, 26.73, 25.57, ...
    ```

    性能下降并不是线性的，4000时的最坏情况就已经和6000相同了。从3500，4000看，不能全入内存时，loop0好像反而是性能更好一点，可能因为第一次只用换出，不用换入，第二次之后就需要换入+换出。但是6000的时候为什么又没有这个特征了呢？
    从3500刚开始执行换入换出的情况来看，置换策略应该不是简单的LRU，因为loop是线性访问，如果是LRU，每次都换入+换出，性能应该更差才对，毕竟机械硬盘的IO速度肯定到不了856MB/s。


7. 没其他设备，SSD应该肯定会快一些吧
