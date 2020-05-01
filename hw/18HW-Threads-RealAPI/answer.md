1. First build main-race.c. Examine the code so you can see the (hopefully
obvious) data race in the code. Now run helgrind (by typing valgrind
--tool=helgrind main-race) to see how it reports the race. Does it
point to the right lines of code? What other information does it give to you?
2. What happens when you remove one of the offending lines of code? Now
add a lock around one of the updates to the shared variable, and then around
both. What does helgrind report in each of these cases?
3. Now let’s look at main-deadlock.c. Examine the code. This code has a
problem known as deadlock (which we discuss in much more depth in a
forthcoming chapter). Can you see what problem it might have?
4. Now run helgrind on this code. What does helgrind report?
5. Now run helgrind on main-deadlock-global.c. Examine the code;
does it have the same problem that main-deadlock.c has? Should helgrind
be reporting the same error? What does this tell you about tools like helgrind?
6. Let’s next look at main-signal.c. This code uses a variable (done) to
signal that the child is done and that the parent can now continue. Why is
this code inefficient? (what does the parent end up spending its time doing,
particularly if the child thread takes a long time to complete?)
7. Now run helgrind on this program. What does it report? Is the code
correct?
8. Now look at a slightly modified version of the code, which is found in
main-signal-cv.c. This version uses a condition variable to do the signaling (and associated lock). Why is this code preferred to the previous
version? Is it correctness, or performance, or both?
9. Once again run helgrind on main-signal-cv. Does it report any errors?
---
1. ```
    ==71== Possible data race during write of size 4 at 0x30A014 by thread #1
    ==71== Locks held: none
    ==71==    at 0x108D30: main (main-race.c:15)
    ==71==
    ==71== This conflicts with a previous write of size 4 by thread #2
    ==71== Locks held: none
    ==71==    at 0x108CDF: worker (main-race.c:8)
    ==71==    by 0x4C36C26: ??? (in /usr/lib/valgrind/vgpreload_helgrind-amd64-linux.so)
    ==71==    by 0x4E496DA: start_thread (pthread_create.c:463)
    ==71==    by 0x518288E: clone (clone.S:95)
    ==71==  Address 0x30a014 is 0 bytes inside data symbol "balance"
    ```
    行号正确，额外信息，Locks held: None

2. 去除之后和添加锁之后，helgrind没有错误输出

3. 1. 请求多个锁资源，加锁顺序交叉，循环等待， 2. 持有锁之后不释放、不抢夺
4. `Thread #3: lock order "0x30A040 before 0x30A080" violated`

5. 本来不应该再报死锁的错误，因为有一个全局锁掌控了加锁的区域。但是helgrind还是报了和4问中几乎相同的死锁的错误警告，只是锁的地址有改变，说明helgrind并不能分析具体场景，似乎之分析加锁释放锁的顺序

6. 主线程一直忙轮询
7. helgrind报了数据竞争的警告，一个是`done`，一个好像是`printf`的，不应该报错吧这个。好像它只管是否多线程访问了同一个内存地址，哪怕全是读他都要报错。
   ```
   ==180== Possible data race during read of size 4 at 0x30A014 by thread #1
    ==180== Locks held: none
    ==180==    at 0x108D83: main (main-signal.c:18)
    ==180==
    ==180== This conflicts with a previous write of size 4 by thread #2
    ==180== Locks held: none
    ==180==    at 0x108D36: worker (main-signal.c:10)
    ==180==    by 0x4C36C26: ??? (in /usr/lib/valgrind/vgpreload_helgrind-amd64-linux.so)
    ==180==    by 0x4E496DA: start_thread (pthread_create.c:463)
    ==180==    by 0x518288E: clone (clone.S:95)
    ==180==  Address 0x30a014 is 0 bytes inside data symbol "done"
    ==180==
    ==180== ----------------------------------------------------------------
    ==180==
    ==180== Possible data race during write of size 1 at 0x5C531A5 by thread #1
    ==180== Locks held: none
    ==180==    at 0x4C3C546: mempcpy (in /usr/lib/valgrind/vgpreload_helgrind-amd64-linux.so)
    ==180==    by 0x50EC993: _IO_file_xsputn@@GLIBC_2.2.5 (fileops.c:1258)
    ==180==    by 0x50E1A8E: puts (ioputs.c:40)
    ==180==    by 0x108D98: main (main-signal.c:20)
    ==180==  Address 0x5c531a5 is 21 bytes inside a block of size 4,096 alloc'd
    ==180==    at 0x4C30F2F: malloc (in /usr/lib/valgrind/vgpreload_helgrind-amd64-linux.so)
    ==180==    by 0x50DF18B: _IO_file_doallocate (filedoalloc.c:101)
    ==180==    by 0x50EF378: _IO_doallocbuf (genops.c:365)
    ==180==    by 0x50EE497: _IO_file_overflow@@GLIBC_2.2.5 (fileops.c:759)
    ==180==    by 0x50EC9EC: _IO_file_xsputn@@GLIBC_2.2.5 (fileops.c:1266)
    ==180==    by 0x50E1A8E: puts (ioputs.c:40)
    ==180==    by 0x108D35: worker (main-signal.c:9)
    ==180==    by 0x4C36C26: ??? (in /usr/lib/valgrind/vgpreload_helgrind-amd64-linux.so)
    ==180==    by 0x4E496DA: start_thread (pthread_create.c:463)
    ==180==    by 0x518288E: clone (clone.S:95)
    ==180==  Block was alloc'd by thread #2
   ```

8. 正确性和性能都会更好。正确性可能要阅读材料中的论文。性能上，会把wait的线程睡眠，避免CPU的使用
9. 不报错
