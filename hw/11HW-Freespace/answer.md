1. First run with the flags -n 10 -H 0 -p BEST -s 0 to generate a few random allocations and frees. Can you predict what alloc()/free() will return? Can you guess the state of the free list after each request? What do you notice about the free list over time?
2. How are the results different when using a WORST fit policy to search the free list (-p WORST)? What changes?
3. What about when using FIRST fit (-p FIRST)? What speeds up when you use first fit?
4. For the above questions, how the list is kept ordered can affect the time it takes to find a free location for some of the policies. Use the different free list orderings (-l ADDRSORT, -l SIZESORT+, -l SIZESORT-) to see how the policies and the list orderings interact.
5. Coalescing of a free list can be quite important. Increase the number of random allocations (say to -n 1000). What happens to larger allocation requests over time? Run with and without coalescing (i.e., without and with the -C flag). What differences in outcome do you see? How big is the free list over time in each case? Does the ordering of the list matter in this case?
6. What happens when you change the percent allocated fraction -P to higher than 50? What happens to allocations as it nears 100? What about as the percent nears 0?
7. What kind of specific requests can you make to generate a highlyfragmented free space? Use the -A flag to create fragmented free lists, and see how different policies and options change the organization of the free list.

---

1. 空闲列表变多，BEST策略的搜索变成本变高
```
[addr-1000, size]
[3, 97]
[0, 3] [3, 97]
[0, 3] [8, 92]
[0, 3] [3, 5] [8, 92]
[0, 3] [3, 5] [16, 84]
[0, 3] [3, 5] [8, 8] [16, 84]
[0, 3] [3, 5] [16, 84]
[0, 3] [3, 5] [8, 8] [16, 84]
[2, 1] [3, 5] [8, 8] [16, 84]
[2, 1] [3, 5] [15, 1] [16, 84]
```

2. 空闲列表再长一寸，每次分配都追加到后面的大片内存区域

3. 和`BEST`的情况相同，但是搜索的次数减少

4. 对于`BEST`和`WORST`，总要全局扫描一次，因此列表顺序无关。但是对`FIRST`，按照size从大到小排，可以减少更多的查找次数，这时前期的分配行为和`WORST`相似

5. 如果不整理碎片，三种方法都会的空闲列表可达30~100的长度，每个size都是1~2。整理碎片后，列表最长的也就是5~6, 有利于查找。在使用-C的情况下，排序非常重要！因此代码里的合并是基于ADDRSORT的，只要使用其他的排序方法，效果都不理想
    ```python
    if self.coalesce == True:
        self.newlist = []
        self.curr    = self.freelist[0]
        for i in range(1, len(self.freelist)):
            eaddr, esize = self.freelist[i]
            if eaddr == (self.curr[0] + self.curr[1]):
                self.curr = (self.curr[0], self.curr[1] + esize)
            else:
                self.newlist.append(self.curr)
                self.curr = eaddr, esize
        self.newlist.append(self.curr)
        self.freelist = self.newlist
    ````

6. P接近100，表示只分配不释放，那空闲列表长度一直都是1，内存耗尽后alloc全部返回-1。P接近0，表示每次使用后立马就释放，对合并的性能要求较高

7. 高度碎片化，比如100的空间，那就是100长度的空闲列表，每个size都是1。就100次次`alloc(1)`，然后全部释放掉,`+1,+1,...+1,-1,-2,-3,-4,...`。前提是关闭`-C`，等效于开启`C`，但是选择**不是**`ADDRSORT`的排序方法
