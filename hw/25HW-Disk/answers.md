1. Compute the seek, rotation, and transfer times for the following sets of requests: -a 0, -a 6, -a 30, -a 7,30,8, and finally -a 10,11,12,13.
2. Do the same requests above, but change the seek rate to different values: -S
2, -S 4, -S 8, -S 10, -S 40, -S 0.1. How do the times change?
3. Do the same requests above, but change the rotation rate: -R 0.1, -R 0.5,
-R 0.01. How do the times change?
4. FIFO is not always best, e.g., with the request stream -a 7,30,8, what order should the requests be processed in? Run the shortest seek-time first
(SSTF) scheduler (-p SSTF) on this workload; how long should it take
(seek, rotation, transfer) for each request to be served?
5. Now use the shortest access-time first (SATF) scheduler (-p SATF). Does it
make any difference for -a 7,30,8 workload? Find a set of requests where
SATF outperforms SSTF; more generally, when is SATF better than SSTF?
6. Here is a request stream to try: -a 10,11,12,13. What goes poorly when
it runs? Try adding track skew to address this problem (-o skew). Given
the default seek rate, what should the skew be to maximize performance?
What about for different seek rates (e.g., -S 2, -S 4)? In general, could
you write a formula to figure out the skew?
7. Specify a disk with different density per zone, e.g., -z 10,20,30, which
specifies the angular difference between blocks on the outer, middle, and
inner tracks. Run some random requests (e.g., -a -1 -A 5,-1,0, which
specifies that random requests should be used via the -a -1 flag and that
five requests ranging from 0 to the max be generated), and compute the
seek, rotation, and transfer times. Use different random seeds. What is the
bandwidth (in sectors per unit time) on the outer, middle, and inner tracks?
8. A scheduling window determines how many requests the disk can examine
at once. Generate random workloads (e.g., -A 1000,-1,0, with different
seeds) and see how long the SATF scheduler takes when the scheduling window is changed from 1 up to the number of requests. How big of a window
is needed to maximize performance? Hint: use the -c flag and don’t turn
on graphics (-G) to run these quickly. When the scheduling window is set
to 1, does it matter which policy you are using?
9. Create a series of requests to starve a particular request, assuming an SATF
policy. Given that sequence, how does it perform if you use a bounded
SATF (BSATF) scheduling approach? In this approach, you specify the
scheduling window (e.g., -w 4); the scheduler only moves onto the next
window of requests when all requests in the current window have been serviced. Does this solve starvation? How does it perform, as compared to
SATF? In general, how should a disk make this trade-off between performance and starvation avoidance?
10. All the scheduling policies we have looked at thus far are greedy; they pick
the next best option instead of looking for an optimal schedule. Can you
find a set of requests in which greedy is not optimal?

---

1.  req | seek | rotation | transfer
    -- | -- | -- | --|
    0 | 0 | 165 | 30
    6 | 0 | 345 | 30
    30 | 80 | 265 | 30
    7,30,8 | 0+80+80 | 15+220+310 | 30+30+30
    10,11,12,13| 0+40 | 105+320 | 60+60

2. seek变快，寻道时间小于15，会让7,30,8请求中30->8少旋转一周，也就是-S4时，寻道时间越快，越不容易"错过"。

3. 旋转变慢，传输变长，也更不容易错过，性能肯定还是变差了，多转一圈都比降速后快

4. 7、8在同一磁道，先服务
    ```
    Block:   7  Seek:  0  Rotate: 15  Transfer: 30  Total:  45
    Block:   8  Seek:  0  Rotate:  0  Transfer: 30  Total:  30
    Block:  30  Seek: 80  Rotate:190  Transfer: 30  Total: 300
    ```

5. `-a 7,30,8`没有区别。`-a 7,5,22`区别明显。在旋转成本明显高于寻道时间的情况下SATF更好
    ```
    $ SATF
    Block:   7  Seek:  0  Rotate: 15  Transfer: 30  Total:  45
    Block:  22  Seek: 40  Rotate: 20  Transfer: 30  Total:  90
    Block:   5  Seek: 40  Rotate:140  Transfer: 30  Total: 210

    TOTALS      Seek: 80  Rotate:175  Transfer: 90  Total: 345

    $ SSTF
    Block:   7  Seek:  0  Rotate: 15  Transfer: 30  Total:  45
    Block:   5  Seek:  0  Rotate:270  Transfer: 30  Total: 300
    Block:  22  Seek: 40  Rotate: 80  Transfer: 30  Total: 150

    TOTALS      Seek: 40  Rotate:365  Transfer: 90  Total: 495
    ```

6. skew=2 `skew * 扇区角度 > 寻道时间`，11->12的寻道时间从350下降到20

7. 外圈0.1, 中圈0.05， 内圈0.033。外圈分配更多扇区，更多的请求集中在一个磁道上，这样感觉会减少一些寻道

8. window=1就是FIFO。最优解的窗口大小当然是任务数，但是实际上比较早就能达到最佳性能
    ```
    ----------------seed 0------------------
    win 1, total:220125
    win 11, total:61755
    win 21, total:47715
    win 31, total:42315
    win 41, total:40005
    win 51, total:39075
    win 61, total:38355
    win 71, total:37635
    win 81, total:39795
    win 91, total:37995
    win 101, total:37275
    win 111, total:36555
    win 121, total:37275
    win 131, total:36195
    win 141, total:35835
    win 151, total:36555
    win 161, total:35835
    win 171, total:36195
    win 181, total:35475
    win 191, total:36555
    win 201, total:35475  ⭐
    win 211, total:35475
    win 221, total:35475
    win 231, total:35475
    win 241, total:35475
    win 251, total:35475
    win 261, total:35475
    win 271, total:35475
    win 281, total:35475
    win 291, total:35475
    ----------------seed 1------------------
    win 1, total:219195
    win 11, total:61575
    win 21, total:49335
    win 31, total:45075
    win 41, total:41205
    win 51, total:39765
    win 61, total:39045
    win 71, total:38985
    win 81, total:37965
    win 91, total:37185
    win 101, total:36525
    win 111, total:36195
    win 121, total:36195
    win 131, total:35835
    win 141, total:35835
    win 151, total:36195
    win 161, total:35835
    win 171, total:35835
    win 181, total:36075
    win 191, total:36195
    win 201, total:36195
    win 211, total:35835
    win 221, total:36195
    win 231, total:35835    ⭐
    win 241, total:35835
    win 251, total:35835
    win 261, total:35835
    win 271, total:35835
    win 281, total:35835
    win 291, total:35835
    ----------------seed 2------------------
    win 1, total:229815
    win 11, total:60375
    win 21, total:46845
    win 31, total:42795
    win 41, total:39885
    win 51, total:37725
    win 61, total:36645
    win 71, total:36645
    win 81, total:35925
    win 91, total:35475
    win 101, total:35475
    win 111, total:35475
    win 121, total:35565
    win 131, total:35475     ⭐
    win 141, total:35475
    win 151, total:35475
    win 161, total:35475
    win 171, total:35475
    win 181, total:35475
    win 191, total:35475
    win 201, total:35475
    win 211, total:35475
    win 221, total:35475
    win 231, total:35475
    win 241, total:35475
    win 251, total:35475
    win 261, total:35475
    win 271, total:35475
    win 281, total:35475
    win 291, total:35475
    ```

9. `-a 17,7,8,9,10,11,0` 17最早请求，但是最后执行。bounded当然可以解决饥饿，本质上和之前做过的`mutex-nostarve`一样，把房间里的请求消耗完。

10. 也就是构造一个greedy执行后，导致"错过"下一个请求的情况，所以`-a 9,20`就可以满足，SATF先执行20，然后因为寻道错过9，多转一圈。`-a 4,12`，SSTF会先执行4，显然也不是最优的
