1. Our first question focuses on main-two-cvs-while.c (the working solution). First, study the code. Do you think you have an understanding of what should happen when you run the program?
2. Run with one producer and one consumer, and have the producer
produce a few values. Start with a buffer (size 1), and then increase
it. How does the behavior of the code change with larger buffers?
(or does it?) What would you predict num full to be with different
buffer sizes (e.g., -m 10) and different numbers of produced items
(e.g., -l 100), when you change the consumer sleep string from
default (no sleep) to -C 0,0,0,0,0,0,1?
3. If possible, run the code on different systems (e.g., a Mac and Linux).
Do you see different behavior across these systems?
4. Let’s look at some timings. How long do you think the following execution, with one producer, three consumers, a single-entry
shared buffer, and each consumer pausing at point c3 for a second, will take? ./main-two-cvs-while -p 1 -c 3 -m 1 -C
0,0,0,1,0,0,0:0,0,0,1,0,0,0:0,0,0,1,0,0,0 -l 10 -v
-t
5. Now change the size of the shared buffer to 3 (-m 3). Will this make
any difference in the total time?
6. Now change the location of the sleep to c6 (this models a consumer taking something off the queue and then doing something
with it), again using a single-entry buffer. What time do you predict in this case? ./main-two-cvs-while -p 1 -c 3 -m 1
-C 0,0,0,0,0,0,1:0,0,0,0,0,0,1:0,0,0,0,0,0,1 -l 10
-v -t
7. Finally, change the buffer size to 3 again (-m 3). What time do you
predict now?
8. Now let’s look at main-one-cv-while.c. Can you configure
a sleep string, assuming a single producer, one consumer, and a
buffer of size 1, to cause a problem with this code?
9. Now change the number of consumers to two. Can you construct
sleep strings for the producer and the consumers so as to cause a
problem in the code?
10. Now examine main-two-cvs-if.c. Can you cause a problem to
happen in this code? Again consider the case where there is only
one consumer, and then the case where there is more than one.
11. Finally, examine main-two-cvs-while-extra-unlock.c. What
problem arises when you release the lock before doing a put or a
get? Can you reliably cause such a problem to happen, given the
sleep strings? What bad thing can happen?

---

1. 主线程启动`p`个生产者线程，`c`个消费者线程，每个生产者往容量为`m`的buffer放置`l`个物品后退出。主线程等待所有生产者退出，再往buffer中追加c个EOF标记，消费者线程看到EOF标记后自动退出，返回消费了多少个物品，主线程最后等待所有消费者线程退出，打印统计数据
2. 基础设置，`-l10 -m1 -p1 -c1 -v -t`。 buffer越大，生产者线程退出得越早。` -C 0,0,0,0,0,0,1` 表示消费者线程拿走数据、释放锁之后睡眠1s，应该是再模拟真正得计算工作。这1s时间内，足够生产者把buffer塞满，所以`num_fill`的序列是`0,1,2,...,l, l-1, l, l-1, l,..., l-1, l-2, ..., 1, 0`
3. 么得Mac
4. c3为1，表示从休眠中醒来后，发呆1s，然后再尝试从buffer中拿走物品，因为发呆期间持有互斥锁，所以全部生产者消费者线程都要等这1s。要知道总的运行时间，就是看消费者会被唤醒多少次。理论上buffer容量为1，生产者、消费者会交替休眠，除非上下文切换的周期恰好是一次循环的时间。因为有trace代码，在我的机器上，-p1 -c1其实就会出现这种情况，只要0.03s，也就是生产者和消费者实际上都没有休眠过。但是该问题中有3个消费者，即使其中一个消费者一直没有休眠，另外两个消费者总会睡去，而生产者放入物品后必然会唤醒其中一个，它被调度后可能发现任务早已被没有休眠的消费者拿走，于是继续睡，也可能自己处理任务，但不管怎样，这1s必然伴随生产者的signal而产生，因此总耗时10+s。最后的数据统计也显示，C2消费了所有物品，从trace记录来看，C2也是根本没有休眠。
   ```
   Consumer consumption:
    C0 -> 0
    C1 -> 0
    C2 -> 10

    Total time: 12.36 seconds
   ```

5. buffer增到3，生产者本来应该一次性放3个，消费者一次性消费三个，总的时间应该小于10。但是我的trace还是10+s，每次生产者往buffer中放入**一个**就上下文切换了，切换到没休眠的消费者，消费一次，又恰好切换到被生产者唤醒的另一个消费者，损失1s，然后再调度生产者放一个物品切换上下文……，和上面的情况一摸一样。也是巧了。


6. c6耽搁1s时，因为没有持有锁，所以其他消费者可以从buffer中拿数据。又因为单位生产时间和单位消费时间相比，几乎忽略不计，所以可以认为c6的休眠是重叠的，于是总耗时减少到了5s，每个消费者也平均地分配了物品。
   ```
   Consumer consumption:
      C0 -> 4
      C1 -> 3
      C2 -> 3

    Total time: 5.04 seconds
   ```

7. 从整体来思考。物品消耗的速度是每秒3个，所以buffer的大小并不影响处理的速度，所以还是和7题相同，5秒。

8. 单生产者单消费者，一个信号量没什么问题
9. 两个消费者，问题会出在唤醒的对象不对。首先来个消费者休眠，然后生产者放入休眠，唤醒一个消费者，消费者处理后本应该唤醒生产者，如果唤醒了另一个消费者就GG。所以 -P 1让生产者后休眠就有机会造成该现象。`./main-one-cv-while -l10 -l1 -p1 -c2 -P 1 -t -v` 消费一次就退出了。
10. 同样在单个消费者时没有问题。if的错误在于生产者执行唤醒，未休眠的消费者在被唤醒消费者之前，把物品消耗了，让被唤醒的消费者发生错误。`./main-two-cvs-if -l3 -m1 -p1 -c2 -P 1 -C 0:1 -t -v`，所以让一个消费者先休眠
11. 故意释放锁，让buffer操作在不受互斥保护，那可以构造这样一种情况，生产者放入物品后休眠，消费者A判断满足条件，执行`do_get`前切换到消费者B，B也检查buffer非空，执行`do_get`，然后切换到A，执行报错。要构建在第一次释放互斥锁后面增加sleep是最稳定复现的。现有的指令造不出来
