1. Examine flag.s. This code “implements” locking with a single memory
flag. Can you understand the assembly?
2. When you run with the defaults, does flag.s work? Use the -M and -R
flags to trace variables and registers (and turn on -c to see their values).
Can you predict what value will end up in flag?
3. Change the value of the register %bx with the -a flag (e.g., -a bx=2,bx=2
if you are running just two threads). What does the code do? How does it
change your answer for the question above?
4. Set bx to a high value for each thread, and then use the -i flag to generate
different interrupt frequencies; what values lead to a bad outcomes? Which
lead to good outcomes?
5. Now let’s look at the program test-and-set.s. First, try to understand
the code, which uses the xchg instruction to build a simple locking primitive. How is the lock acquire written? How about lock release?
6. Now run the code, changing the value of the interrupt interval (-i) again,
and making sure to loop for a number of times. Does the code always work
as expected? Does it sometimes lead to an inefficient use of the CPU? How
could you quantify that?
7. Use the -P flag to generate specific tests of the locking code. For example,
run a schedule that grabs the lock in the first thread, but then tries to acquire
it in the second. Does the right thing happen? What else should you test?
8. Now let’s look at the code in peterson.s, which implements Peterson’s
algorithm (mentioned in a sidebar in the text). Study the code and see if
you can make sense of it.
9. Now run the code with different values of -i. What kinds of different behavior do you see? Make sure to set the thread IDs appropriately (using -a
bx=0,bx=1 for example) as the code assumes it.
10. Can you control the scheduling (with the -P flag) to “prove” that the code
works? What are the different cases you should show hold? Think about
mutual exclusion and deadlock avoidance.
11. Now study the code for the ticket lock in ticket.s. Does it match the code
in the chapter? Then run with the following flags: -a bx=1000,bx=1000
(causing each thread to loop through the critical section 1000 times). Watch
what happens; do the threads spend much time spin-waiting for the lock?
12. How does the code behave as you add more threads?
13. Now examine yield.s, in which a yield instruction enables one thread
to yield control of the CPU (realistically, this would be an OS primitive, but
for the simplicity, we assume an instruction does the task). Find a scenario
where test-and-set.s wastes cycles spinning, but yield.s does not.
How many instructions are saved? In what scenarios do these savings arise?
14. Finally, examine test-and-test-and-set.s. What does this lock do?
What kind of savings does it introduce as compared to test-and-set.s?
---

1. 共享flag内存，为0时进入临界区，把count加1
2. 默认设置都不会切换上下文，正常，count为2
3. 多循环一次，还是不切上下文，count为4
4. i1 i2 i3 i4等都会造成有问题的结果，因为循环中还会释放锁，导致加锁的的中间状态暴露无遗
5. `test-and-set`的原则是，每次都会原子更新，更新后检测旧值
   ```c
    //lock
    while (test_and_set(address, state_new) == state_old)
        ;
    //unlock
    *address = state_old
    ```
6. 总能按照预期工作。用(用于获取锁的时间片/总的时间片)来定义获取锁的代价，越高则CPU的使用越低效
7. ❓如果每个线程依次执行一个指令，能有确定的行为，那就是没有问题的多线程❓可以这样说吗？我觉得可以，前提是，两个线程的起点相同，执行的指令也一样，这样临界区一定在x=y直线上，再依次执行，就是沿x=y挪动，这都没问题，那么临界区就小于单位面积，无论如何都进入不了临界区。所以对这个题目，不用-P测试多少，-i1可以就说明锁的正确性了
8. flag[0] 表示thread0准备或已经进入临界区；flag[1]表示thread1准备或已经进入临界区。trun表示现在真正该工作的是谁。线程进入lock，先用flag表明意图，但是会先礼让，把trun给对方，然后检查，如果对方有意向工作、并且turn也属于它，自己就等待。在双方都有意向时，后表达意图的会把turn礼让给先表达意图的，但总归只有一方会得到turn。关键就在于这个礼让，你要是整个turn=self，后来的线程还是会进入临界区，没有互斥的效果。
9. 临界区的count设置只有一个活动的线程，count总能被正确地更新到2
10. 单个锁不会死锁。两套锁，先后加，还是能出现死锁。
    ```
    ^表示turn，o表示有意向，x表示没意向
    step0
    ^       ^
    x x     x x

    step1  thread0给lock0加锁，锁上没有对方意向，成功返回
      ^     ^
    o x     x x

    step2  thread1给lock1加锁，锁上没有对方意向，成功返回
      ^     ^
    o x     x o

    step3  thread0给lock1加锁，但是turn属于thread1，于是阻塞在lock1的lock调用中
      ^       ^
    o x     o o

    step4  thread1给lock0加锁，但是turn属于thread0，阻塞在lock0的lock调用中
    ^         ^
    o o     o o
    然后就一直无限循环下去了
    ```
11. 这里的FetchAndAdd会接受一个寄存器src，加数不单单是1，而是src的值，返回的old结果也放到src寄存器上
12. 因为ticket有公平的特性，所以会出现连续两个时间片都在空转的情形。t0正在执行临界区，t1进来，ticket+1，然后在自己的时间片里等待；t0出临界区，turn+1，接着ticket+1，在本次时间片中发现ticket > turn，所以一也待，切换到t1时才能执行

13. 添加更多的线程，会有更多的线程在空转，因为只有其中1个被调度才能执行。N的线程，可能N-1次调度都是空转
14. yield是放弃当前时间片后续执行。所以在i参数比较大的时候有节省的效果。
15. 在执行原子操作前，先检测锁是不是已经被别人占用了，只发现没人占用时才执行test-and-set，这样可以减少test-and-set这种复杂指令的执行，提高效率
