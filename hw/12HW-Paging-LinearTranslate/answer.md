1. Before doing any translations, let’s use the simulator to study how
linear page tables change size given different parameters. Compute
the size of linear page tables as different parameters change. Some
suggested inputs are below; by using the -v flag, you can see
how many page-table entries are filled. First, to understand how
linear page table size changes as the address space grows, run with
these flags:
    - -P 1k -a 1m -p 512m -v -n 0
    - -P 1k -a 2m -p 512m -v -n 0
    - -P 1k -a 4m -p 512m -v -n 0

    Then, to understand how linear page table size changes as page size
grows:
    - -P 1k -a 1m -p 512m -v -n 0
    - -P 2k -a 1m -p 512m -v -n 0
    - -P 4k -a 1m -p 512m -v -n 0

    Before running any of these, try to think about the expected trends.
How should page-table size change as the address space grows? As
the page size grows? Why not use big pages in general?

2. Now let’s do some translations. Start with some small examples,
and change the number of pages that are allocated to the address
space with the -u flag. For example:
    - -P 1k -a 16k -p 32k -v -u 0
    - -P 1k -a 16k -p 32k -v -u 25
    - -P 1k -a 16k -p 32k -v -u 50
    - -P 1k -a 16k -p 32k -v -u 75
    - -P 1k -a 16k -p 32k -v -u 100

    What happens as you increase the percentage of pages that are allocated in each address space?
3. Now let’s try some different random seeds, and some different (and
sometimes quite crazy) address-space parameters, for variety:
    - -P 8 -a 32 -p 1024 -v -s 1
    - -P 8k -a 32k -p 1m -v -s 2
    - -P 1m -a 256m -p 512m -v -s 3

    Which of these parameter combinations are unrealistic? Why?
4. Use the program to try out some other problems. Can you find the
limits of where the program doesn’t work anymore? For example,
what happens if the address-space size is bigger than physical memory?

---

1. 页大小1K不变，低10位一直是offset，随着虚拟地址空间的增加，多出来的高位都会当作VPN，所以页表的大小会增加，表项由1K -> 2K -> 4K; 相反虚拟地址空间不变，页容量变大，表项会减少，1K -> 512 -> 256。页容量不要太大，因为会产生较多的内部碎片
2. u0，最多0%的地址进入主存；u25, 最多50%的地址进入主存; u50~100, 最多100%的地址进入主存
3. 1m的page太大了？
4. `Error: physical memory size must be GREATER than address space size (for this simulation)`  
   `Error in argument: page size must be a power of 2`
