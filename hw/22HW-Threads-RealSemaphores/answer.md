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
   1. 读者如果一直存在，或者说，前后脚的申请加锁，就会让写者饥饿，比如，15个读者，每隔0.1秒加锁，每个读者工作0.3s，那么0~1.7s内，写者都不能加上写锁，哪怕写锁是在0.2s到来的，运行`./reader-writer 20 1 1`就会看到，写者一定最后打印
   2. 写者获得了工作权限，执行完毕，释放锁时，post的是`writelock`，在这个信号量上休眠的，可能有一个读者和多个写者，那么唤醒谁，从信号量的定义来说，并不确定。假设是随机的，那写者多，概率大，如果有100个写者，那么唯一的读者也会饥饿，所以看上去的读者优先也是不可靠的

5. 这里要处理饥饿问题，先看了第6题，也相当于提示，构造一个mutex，让休眠在这个mutex上的线程，都能保证得到调度。这种公平性，只在`fetch-and-add`的模式上见过，本质就是大家拿着爱的号码牌，叫谁谁就来，不要搞裙带关系。
   对应到读写锁上，理想中的情况应该是，按照获取读写锁的绝对时间，假设得到的序列如下:
   ```
   rrr w w rrrrr w rrrrrr
   ```
   那么执行情况应该是，前3个读者并发，串行执行两2个写者，5个读者并发，执行1个写者，最后6个读者并发。

   因此，想法和`fetch-and-add`一致，一个线程的`token`和`turn`相等时，可以执行。`token`在一个写者到来时增加，读者到来时沿用，不增加，`turn`在写者执行完后，或者当前所有读者执行完后增加。但是这个用条件变量的notify_all好像比较好做，用信号量就不太会。然后就放弃了🙃

   到github上搜了一下第5题，看了[xxyzz](https://github.com/xxyzz/ostep-hw/blob/master/31/reader-writer-nostarve.c)的解法。妙啊。在最外层加一个检验机制，连续的wait和post就可以看成一种使用方法，不改变条件。
   ```
   Sem_wait(reader_lock);
   Sem_post(reader_lock);
   ```
   先wait，之后再post是另一种方法，属于改变条件的。在获取写锁时，需要wait `reader_lock` 和 `writer_lock`。前者是阻塞后序到来的读者，后者是等待前面的读者。顺序很重要，一定要先`wait(reader_lock)`，否则当写者到来，休眠在`writer_lock`上时，并没有阻碍后续的读者到来。

   然后我感觉这个解法对第二个问题也有有改善。当写者释放时，`reader_lock`上可能有多个读者多个写者，读者被唤醒的机会更大了。但是它也不是理想中的情况，它属于抽奖型的，比如序列`rrr w (rrrr w rrr w rrr)`，括号里元素的任意一种排列都可以成为后续的执行。如果是读者被唤醒，马上他就再post `reader_lock`，如果再唤醒读者，那么就并发执行，所以极端情况下，它可以只唤醒读者。

6. *[Little Book Of Semaphores](https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf)* 的第4.3节，介绍了解法。原来师爷Dijkstra也曾今认为没有饥饿的实现要靠FIFO的队列。

   Morris的解法如下：
   ```python
   # init
   room1 = room2 = 0
   mutex = Semaphore(1)
   t1 = Semaphore(1)
   t2 = Semaphore(0)

   mutex.wait()                   [1]
   room1 += 1                     [2]
   mutex.signal()                 [3]
   t1.wait()                      [4]
      room2 += 1                  [5]
      mutex.wait()                [6]
      room1 -= 1                  [7]
      if room1 == 0:              [8]
         mutex.signal()           [9]
         t2.signal()              [10]
      else :                      [11]
         mutex.signal()           [12]
         t1.signal()              [13]
   t2.wait()                      [14]
      room2 -= 1                  [15]
      # critical section          [**]
      if room2 == 0:              [16]
         t1.signal()              [17]
      else :                      [18]
         t2.signal()              [19]
   ```
   单个线程，t1是打开的，关上t1后，顺利到达[5]处，进入了room2，[8]处检查room1，自己是room1的最后一个人，打开t2，顺利通过t2, 关上t2，进入关键区域。离开关键区域时，发现自己时room2的最后一个人，打开t1。这样就恢复到最初的状态了，两个room为空，t1打开，t2关闭。  
   多个线程时，领头的线程进入了[5]，其他的线程此时都在t1处等着，在room2中，领头线程发现room1不为空，然后打开t1，让其他线程进入room2。现在t2依然是关着的，没有线程可以进入关键区域。又因为这一批并发的线程是有限的，所以他们总会从room1进入room2，那最后从room1进入room2的线程，会打开一下t2，让之前进入room2的某个线程进入关键区域，然后自己等待在t2上，注意此时它还关闭着t1,下一批次的并发线程会等在t1处，所以这个线程简直高风亮节。线程离开关键区域后，检查room2是否为空，不空，就让room2中任意一个线程来关键区域。如果room2为空，就把t1打开，让下一个批次的线程进入room2。  
   太猛了这个！精细！按照并发的密集程度，把线程分为有限的若干批次，每个批次进入room2后，就由信号量一个个挑进关键区域，因为挑选的池子是有限的，用一个少一个，所以必然不会产生饥饿。而在挑选的过程中，下一个批次都等在room1中。挑选完成后，又打开t1，让他们进入room2。

   [6]处虽然在t1的保护之下，只有一个线程在活动，但是他要修改的变量room1还会被t1之外的任意线程锁修改，所以也要获取mutex。[5][15]处的room2的修改要么在t1保护下，要么在t2保护下，任意时刻最多只有一个线程修改，所以不用mutex。

