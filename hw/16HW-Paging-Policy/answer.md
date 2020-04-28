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
4. b的情况真的看不出什么差别，首先80-20的工作负载，是5、15、25...cache size
   ```
    ('-b1: ', ['16.00', '42.00', '60.50', '72.00', '73.50', '75.00', '75.00', '75.00', '75.00', '75.00'])
    ('-b2: ', ['14.00', '44.50', '64.50', '73.50', '74.50', '75.00', '75.00', '75.00', '75.00', '75.00'])
    ('-b3: ', ['12.00', '46.00', '66.00', '74.00', '74.50', '75.00', '75.00', '75.00', '75.00', '75.00'])
   ```
   然后把size的尺度细化到1, 3, 5, 7, 9, ... 29
   ```
   ('-b1: ', ['3.00', '8.00', '16.00', '19.50', '29.50', '38.00', '39.50', '42.00', '51.00', '51.50', '55.00', '60.00', '60.50', '63.00', '67.50'])
    ('-b2: ', ['3.00', '8.00', '14.00', '21.00', '25.00', '38.00', '42.00', '44.50', '52.00', '54.00', '60.50', '65.00', '64.50', '68.50', '68.00'])
    ('-b3: ', ['3.00', '8.50', '12.00', '21.00', '26.50', '39.00', '41.00', '46.00', '50.50', '57.50', '62.00', '63.00', '66.00', '67.50', '71.00'])
   ```
    还是没有看出什么规律，b2看上去在10~30期间的命中率更高一点。b3也并没有比b2更好，理论上多bit的历史更为准确。可能这组输入的问题吧，毕竟随机
