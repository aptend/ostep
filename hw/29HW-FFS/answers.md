1. Examine the file in.largefile, and then run the simulator with flag -f
in.largefile and -L 4. The latter sets the large-file exception to 4 blocks.
What will the resulting allocation look like? Run with -c to check.
2. Now run with -L 30. What do you expect to see? Once again, turn on -c
to see if you were right. You can also use -S to see exactly which blocks
were allocated to the file /a.
3. Now we will compute some statistics about the file. The first is something
we call filespan, which is the max distance between any two data blocks of
the file or between the inode and any data block. Calculate the filespan of
/a. Run ffs.py -f in.largefile -L 4 -T -c to see what it is. Do
the same with -L 100. What difference do you expect in filespan as the
large-file exception parameter changes from low values to high values?
4. Now let’s look at a new input file, in.manyfiles. How do you think the
FFS policy will lay these files out across groups? (you can run with -v to
see what files and directories are created, or just cat in.manyfiles). Run
the simulator with -c to see if you were right.
5. A metric to evaluate FFS is called dirspan. This metric calculates the spread
of files within a particular directory, specifically the max distance between
the inodes and data blocks of all files in the directory and the inode and data
block of the directory itself. Run with in.manyfiles and the -T flag, and
calculate the dirspan of the three directories. Run with -c to check. How
good of a job does FFS do in minimizing dirspan?
6. Now change the size of the inode table per group to 5 (-i 5). How do you
think this will change the layout of the files? Run with -c to see if you were
right. How does it affect the dirspan?
7. Which group should FFS place inode of a new directory in? The default
(simulator) policy looks for the group with the most free inodes. A different
policy looks for a set of groups with the most free inodes. For example, if
you run with -A 2, when allocating a new directory, the simulator will look
at groups in pairs and pick the best pair for the allocation. Run ./ffs.py
-f in.manyfiles -i 5 -A 2 -c to see how allocation changes with
this strategy. How does it affect dirspan? Why might this policy be good?
8. One last policy change we will explore relates to file fragmentation. Run
./ffs.py -f in.fragmented -v and see if you can predict how the
files that remain are allocated. Run with -c to confirm your answer. What
is interesting about the data layout of file /i? Why is it problematic?
9. A new policy, which we call contiguous allocation (-C), tries to ensure that
each file is allocated contiguously. Specifically, with -C n, the file system
tries to ensure that n contiguous blocks are free within a group before allocating a block. Run ./ffs.py -f in.fragmented -v -C 2 -c to
see the difference. How does layout change as the parameter passed to -C
increases? Finally, how does -C affect filespan and dirspan?


----

建议开启`-MSB`观察辅助信息

1. ```
    group inodes     data
        0 /a-------- /aaaa----- ---------- ----------
        1 ---------- aaaa------ ---------- ----------
        2 ---------- aaaa------ ---------- ----------
        3 ---------- aaaa------ ---------- ----------
        4 ---------- aaaa------ ---------- ----------
        5 ---------- aaaa------ ---------- ----------
        6 ---------- aaaa------ ---------- ----------
        7 ---------- aaaa------ ---------- ----------
        8 ---------- aaaa------ ---------- ----------
        9 ---------- aaaa------ ---------- ----------
    ```

2. group0被塞满，剩下的全在group1
    ```
    group inodes     data
        0 /a-------- /aaaaaaaaa aaaaaaaaaa aaaaaaaaaa
        1 ---------- aaaaaaaaaa a--------- ----------
        2 ---------- ---------- ---------- ----------
        3 ---------- ---------- ---------- ----------
        4 ---------- ---------- ---------- ----------
        5 ---------- ---------- ---------- ----------
        6 ---------- ---------- ---------- ----------
        7 ---------- ---------- ---------- ----------
        8 ---------- ---------- ---------- ----------
        9 ---------- ---------- ---------- ----------
    ```

3. L越大，file占据的data block的范围就越少，分配得越紧密

4. 常规原则，分散目录，同目录文件在一组
    ```
    group inodes     data
        0 /abcdefghi /aabbccdde effgghhii- ----------
        1 jlmnopqrC- jlmnopqrCC C--------- ----------
        2 tuvwxyzAB- tuuuvvvwww xxxyyyzzzA AABBB-----
        3 ---------- ---------- ---------- ----------
        4 ---------- ---------- ---------- ----------
        5 ---------- ---------- ---------- ----------
        6 ---------- ---------- ---------- ----------
        7 ---------- ---------- ---------- ----------
        8 ---------- ---------- ---------- ----------
        9 ---------- ---------- ---------- ----------
    ```

5. dirspan，目录inode和目录中所有文件的inode、block的最大距离。因为都放在同一组了，就是组的长度
    ```
    span: directories
    dir:           /  dirspan:  28
    dir:          /t  dirspan:  34
    dir:          /j  dirspan:  20
                avg  dirspan:  27.33
    ```

6. -i 5 可用inodes数减少，一组装不下全部文件了，所以dirspan必然增大
    ```
    dir:           /  dirspan:  49
    dir:          /t  dirspan:  78
    dir:          /j  dirspan: 116
                avg  dirspan:  81.00
    ```

7. 默认-A1
    ```
    0 /abcd /aabbccdd- ---------- ----------
    1 efghi eeffgghhii ---------- ----------
    2 jlmno jlmno----- ---------- ----------
    3 tuvwx tuuuvvvwww xxx------- ----------
    4 ypzqA yyypzzzqAA A--------- ----------
    5 rBC-- rBBBCCC--- ---------- ----------
    6 ----- ---------- ---------- ----------
    7 ----- ---------- ---------- ----------
    8 ----- ---------- ---------- ----------
    9 ----- ---------- ---------- ----------
    ```
    -A2
    ```
    0 /abcd /aabbccdd- ---------- ----------
    1 efghi eeffgghhii ---------- ----------
    2 jlmno jlmno----- ---------- ----------
    3 pqrC- pqrCCC---- ---------- ----------
    4 tuvwx tuuuvvvwww xxx------- ----------
    5 yzAB- yyyzzzAAAB BB-------- ----------
    6 ----- ---------- ---------- ----------
    7 ----- ---------- ---------- ----------
    8 ----- ---------- ---------- ----------
    9 ----- ---------- ---------- ----------
    ```
    -A3
    ```
    0 /abcd /aabbccdd- ---------- ----------
    1 efghi eeffgghhii ---------- ----------
    2 ----- ---------- ---------- ----------
    3 jlmno jlmno----- ---------- ----------
    4 pqrC- pqrCCC---- ---------- ----------
    5 ----- ---------- ---------- ----------
    6 tuvwx tuuuvvvwww xxx------- ----------
    7 yzAB- yyyzzzAAAB BB-------- ----------
    8 ----- ---------- ---------- ----------
    9 ----- ---------- ---------- ----------
    ```

    ⭐每次以A个组为单位去检查最少的inode的使用，这实际上把目录处理得更分散，相当于把A各组得位置都预留给一个目录。

8. 引文小文件的删除，文件i会去填补空洞，导致自己的block碎片化，不利用IO
    ```
    0 /ib-d-f-h- /ibidifihi iii------- ----------
    1 ---------- ---------- ---------- ----------
    ```

9. -C x表示文件的block后不去填补长为x的空洞，直接在同组的后续分配block，x越高，block连续的可能性越高。当然这会导致提高filespan和dirspan。
    ```
    0 /ib-d-f-h- /-b-d-f-hi iiiiiii--- ----------
    1 ---------- ---------- ---------- ----------
    ```
