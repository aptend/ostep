1. With a linear page table, you need a single register to locate the
page table, assuming that hardware does the lookup upon a TLB
miss. How many registers do you need to locate a two-level page
table? A three-level table?
2. Use the simulator to perform translations given random seeds 0,
1, and 2, and check your answers using the -c flag. How many
memory references are needed to perform each lookup?
3. Given your understanding of how cache memory works, how do
you think memory references to the page table will behave in the
cache? Will they lead to lots of cache hits (and thus fast accesses?)
Or lots of misses (and thus slow accesses)

---

1. 1个。定位到顶级目录，之后的PFN都可以按图索骥
2. ```
    seed=0：
    611c -> 0 11000 01000 11100
               24     8     28
    基地址108，第25个  a1 -> 1 0100001 -> 33
    地址33，   第9个   b5 -> 1 0110101 -> 53 实际地址 0110101 11100
    地址53，   第29个  08 

    所以611c的值为08
    ```

    模拟器是二级页表，每次使用3次内存访问到最终的字节位置

3. 地址翻译缓存的内容是基于PTE的，命中与否和页表的结构无关，只是在未命中时，页表的层级越深，更新缓存的时间越长。
