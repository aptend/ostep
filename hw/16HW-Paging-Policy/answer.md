1. Generate random addresses with the following arguments: -s 0
-n 10, -s 1 -n 10, and -s 2 -n 10. Change the policy from
FIFO, to LRU, to OPT. Compute whether each access in said address
traces are hits or misses.
2. For a cache of size 5, generate worst-case address reference streams
for each of the following policies: FIFO, LRU, and MRU (worst-case
reference streams cause the most misses possible. For the worst case
reference streams, how much bigger of a cache is needed to improve
performance dramatically and approach OPT?
3. Generate a random trace (use python or perl). How would you
expect the different policies to perform on such a trace?
4. Now generate a trace with some locality. How can you generate
such a trace? How does LRU perform on it? How much better than
RAND is LRU? How does CLOCK do? How about CLOCK with
different numbers of clock bits?
5. Use a program like valgrind to instrument a real application and
generate a virtual page reference stream. For example, running
valgrind --tool=lackey --trace-mem=yes ls will output
a nearly-complete reference trace of every instruction and data reference made by the program ls. To make this useful for the simulator above, you’ll have to first transform each virtual memory
reference into a virtual page-number reference (done by masking
off the offset and shifting the resulting bits downward). How big
of a cache is needed for your application trace in order to satisfy a
large fraction of requests? Plot a graph of its working set as the size
of the cache increases.


--- 
1. 略了
2. 对FIFO、LRU，最差的情况就是顺序访问了，所以只要超过缓存容量的序列就可以使他们的命中率为0， `1 2 3 4 5 6 1 2 3 4 5 6`。但是在同样顺序的情况下，MRU反而会有41.67的命中率，比如6到来，驱逐的是5，反而这是OPT策略下才会做出的决定，对MRU就要反复横跳避免命中，`1 2 3 4 5 6 5 6 5 6 5 6`。最差情况下，都要把distinct的数全部放入缓存中才可以避免
3. 随机的情况下，没有历史特征，LRU、随机、FIFO都没啥差别
4. 80-20的工作负载，100个page，2000次访问，cache size为5, 15, 25, ... 95
    ```
    ('cache size', [5, 15, 25, 35, 45, 55, 65, 75, 85, 95])
    ('OPT', ['40.70', '71.30', '83.20', '88.00', '90.80', '92.40', '93.65', '94.40', '94.90', '95.05'])
    ('LRU', ['16.20', '45.90', '67.25', '78.90', '84.20', '87.30', '89.65', '91.65', '93.35', '94.65'])
    ('RAND', ['15.60', '41.75', '60.85', '70.50', '75.90', '82.70', '86.50', '91.35', '92.50', '94.85'])
    ```
    在历史有规律的情况下，LRU在中期比RAND好7~10%  
    b的情况，80-20的工作负载，100个page，2000次访问
   ```
    ('cache size', [1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34, 37])
    ('CLOCK -b1', ['3.15', '11.90', '21.30', '29.85', '38.15', '46.35', '52.10', '57.95', '62.10', '65.90', '70.40', '72.10', '75.75'])
    ('CLOCK -b2', ['3.15', '12.50', '21.55', '31.65', '40.45', '47.50', '54.70', '60.10', '64.60', '70.15', '73.35', '76.20', '78.20'])
    ('CLOCK -b3', ['3.15', '12.45', '21.50', '31.65', '39.55', '48.10', '55.75', '62.30', '66.00', '72.10', '75.85', '78.00', '80.30'])
   ```
    位数越多，效果会好一点点(1~2%)，对于昂贵的未命中，还是有实在的价值

5. `getconf PAGE_SIZE` -> 4096 -> 低12位就页内offset  
   `valgrind --tool=lackey --trace-mem=yes --log-fd=1 ls | python convert.py > addr` -> 得到十进制的vpn
   `python compare.py 2 addr` 执行对比
    ```
    ('cache size', [1, 2, 3, 4, 5, 6, 7, 8, 9])
    ('LRU', ['49.50', '83.68', '89.29', '91.85', '93.54', '94.82', '95.65', '96.25', '96.68'])
    ('RAND', ['49.50', '77.42', '85.81', '89.46', '91.74', '93.02', '94.01', '94.74', '95.37'])
    ```
    OPT的时间复杂度是平方，80万的输入顶不住。从LRU和RAND看，ls的局部性只要4，就能达到90%的命中率
    ```
    ('cache size', [10, 11, 12, 13, 14, 15, 16, 17, 18, 19])
    ('LRU', ['96.94', '97.22', '97.49', '97.75', '98.03', '98.23', '98.43', '98.54', '98.66', '98.75'])
    ('RAND', ['95.88', '96.29', '96.68', '96.98', '97.31', '97.54', '97.76', '97.93', '98.10', '98.29'])
    ```
    再10组，100%不是梦
