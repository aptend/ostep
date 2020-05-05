1. The first problem is just to implement and test a solution to the fork/join
problem, as described in the text. Even though this solution is described in
the text, the act of typing it in on your own is worthwhile; even Bach would
rewrite Vivaldi, allowing one soon-to-be master to learn from an existing
one. See fork-join.c for details. Add the call sleep(1) to the child to
ensure it is working.
2. Let’s now generalize this a bit by investigating the rendezvous problem.
The problem is as follows: you have two threads, each of which are about
to enter the rendezvous point in the code. Neither should exit this part of
the code before the other enters it. Consider using two semaphores for this
task, and see rendezvous.c for details.
3. Now go one step further by implementing a general solution to barrier synchronization. Assume there are two points in a sequential piece of code,
called P1 and P2. Putting a barrier between P1 and P2 guarantees that all
threads will execute P1 before any one thread executes P2. Your task: write
the code to implement a barrier() function that can be used in this manner. It is safe to assume you know N (the total number of threads in the
running program) and that all N threads will try to enter the barrier. Again,
you should likely use two semaphores to achieve the solution, and some
other integers to count things. See barrier.c for details.
4. Now let’s solve the reader-writer problem, also as described in the text. In
this first take, don’t worry about starvation. See the code in reader-writer.c
for details. Add sleep() calls to your code to demonstrate it works as you
expect. Can you show the existence of the starvation problem?
5. Let’s look at the reader-writer problem again, but this time, worry about
starvation. How can you ensure that all readers and writers eventually
make progress? See reader-writer-nostarve.c for details.
6. Use semaphores to build a no-starve mutex, in which any thread that tries to
acquire the mutex will eventually obtain it. See the code in mutex-nostarve.c
for more information.
7. Liked these problems? See Downey’s free text for more just like them. And
don’t forget, have fun! But, you always do when you write code, no?

--- 

1. s初始化容量为0，主线程wait，子线程post
2. 两个子线程的同步，沿用上题，child1 post s2，同时wait s1，child2做对称的事情
3. 提示了还是用两个信号量实现。假设其中一个信号量上已经等待了N-1个线程，最后一个线程来，将发起N次post，所以另一个信号量当作互斥锁，保证变量更新和比较的原子性。
4. 嗯，照着教材的写法走。`lock`信号量用作互斥锁，保护`reader`数值的更新，`writelock`用作条件变量，存在读者时，上锁，这样写者就会在`writelock`上等待。全部读者unlock时，post `writelock`，让写者可以开始工作。这个解法有两个问题：
   1. 读者如果一直存在，或者说，前后脚的申请加锁，就会让写者饥饿，比如`./reader-writer 100 1 1`就会看到，写者一定是最后打印的
   2. 写者获得了工作权限，执行完毕，释放锁时，post的是`writelock`，在这个信号量上休眠的，可能有一个读者和多个写者，那么唤醒谁，从信号量的定义来说，并不确定。假设是随机的，那写者多，概率大，如果有100个写者，那么唯一的读者也会饥饿，所以看上去的读者优先也是虚假的
5. 这里要处理饥饿问题，先看了第6题，也相当于提示了，构造一个mutex，让休眠在这个mutex上的线程，都能保证得到调度。这种公平性，只在`fetch-and-add`的模式上见过，本质就是大家拿着爱的号码牌，叫道谁谁就来，不要搞裙带关系。
   对应到读写锁上，理想中的情况应该是，按照获取读写锁的绝对时间，假设得到的序列如下:
   ```
   rrr w w rrrrr w rrrrrr
   ```
   那么执行情况应该是，前3个读者并发，串行执行两2个写者，5个读者并发，执行1个写者，最后6个读者并发。

   因此，想法和`fetch-and-add`一致，一个线程的`token`和`turn`相等时，可以执行。`token`在一个写者到来时增加，读者到来时沿用，不增加，`turn`在写者执行完后，或者当前所有读者执行完后增加。
