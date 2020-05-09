
1. Run ./lfs.py -n 3, perhaps varying the seed (-s). Can you figure out which commands were run to generate the final file system
contents? Can you tell which order those commands were issued?
Finally, can you determine the liveness of each block in the final
file system state? Use -o to show which commands were run, and
-c to show the liveness of the final file system state. How much
harder does the task become for you as you increase the number of
commands issued (i.e., change -n 3 to -n 5)?
    
    `creat('/ku3') -> lseek('/ku3', 7*block_size); write('/ku3', 'blablabla...') -> creat('/qg9')`

2. If you find the above painful, you can help yourself a little bit by
showing the set of updates caused by each specific command. To do
so, run ./lfs.py -n 3 -i. Now see if it is easier to understand
what each command must have been. Change the random seed to
get different commands to interpret (e.g., -s 1, -s 2, -s 3, etc.).

    - s1: `creat('/tg4') -> lseek('/tg4', 6*block_size) -> creat('/lt0')`
    - ...

3. To further test your ability to figure out what updates are made to
disk by each command, run the following: ./lfs.py -o -F -s
100 (and perhaps a few other random seeds). This just shows a
set of commands and does NOT show you the final state of the file
system. Can you reason about what the final state of the file system
must be?
    ```
    [   0 ]  ?   checkpoint: 15 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    [   1 ]  ?   [.,0] [..,0] -- -- -- -- -- --
    [   2 ]  ?   type:dir size:1 refs:2 ptrs: 1 -- -- -- -- -- -- --
    [   3 ]  ?   chunk(imap): 2 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    [   4 ]  ?   [.,0] [..,0] [us7,1] -- -- -- -- --
    [   5 ]  ?   type:dir size:1 refs:2 ptrs: 4 -- -- -- -- -- -- --
    [   6 ]  ?   type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
    [   7 ]  ?   chunk(imap): 5 6 -- -- -- -- -- -- -- -- -- -- -- -- -- --

    [   8 ]  ?   type:reg size:4 refs:1 ptrs: -- -- -- -- -- -- -- --
    [   9 ]  ?   chunk(imap): 5 8 -- -- -- -- -- -- -- -- -- -- -- -- -- --
  
    [  10 ]  ?   blablabla
    [  11 ]  ?   type:reg size:8 refs:1 ptrs: -- -- -- -- -- -- -- 10
    [  12 ]  ?   chunk(imap): 5 11 -- -- -- -- -- -- -- -- -- -- -- -- -- --
    ```
4. Now see if you can determine which files and directories are live
after a number of file and directory operations. Run tt ./lfs.py
-n 20 -s 1 and then examine the final file system state. Can you
figure out which pathnames are valid? Run tt ./lfs.py -n 20
-s 1 -c -v to see the results. Run with -o to see if your answers
match up given the series of random commands. Use different random seeds to get more problems.
    
    从imap出发，找活着的inode，再顺着找ptrs就可以了

5. Now let’s issue some specific commands. First, let’s create a file
and write to it repeatedly. To do so, use the -L flag, which lets you
specify specific commands to execute. Let’s create the file ”/foo”
and write to it four times:
-L c,/foo:w,/foo,0,1:w,/foo,1,1:w,/foo,2,1:w,/foo,3,1
-o. See if you can determine the liveness of the final file system
state; use -c to check your answers.
    
    方法同4题没有区别

6. Now, let’s do the same thing, but with a single write operation instead of four. Run ./lfs.py -o -L c,/foo:w,/foo,0,4 to
create file ”/foo” and write 4 blocks with a single write operation. Compute the liveness again, and check if you are right with -c.
What is the main difference between writing a file all at once (as
we do here) versus doing it one block at a time (as above)? What
does this tell you about the importance of buffering updates in main
memory as the real LFS does?

    一次性写入，只有一次inode imap的更新，文件系统中数据block的比重会增大

7. Let’s do another specific example. First, run the following: ./lfs.py
-L c,/foo:w,/foo,0,1. What does this set of commands do?
Now, run ./lfs.py -L c,/foo:w,/foo,7,1. What does this
set of commands do? How are the two different? What can you tell
about the size field in the inode from these two sets of commands?

    创建foo,并且从 offset 0 开始写入一个block  
    创建foo，并且从 offset 7*block_size 开始写入一个block

8. Now let’s look explicitly at file creation versus directory creation.
Run simulations ./lfs.py -L c,/foo and ./lfs.py -L d,/foo
to create a file and then a directory. What is similar about these runs,
and what is different?

    ```
    ...
    [   4 ]  ?   [.,0] [..,0] [foo,1] -- -- -- -- --
    [   5 ]  ?   type:dir size:1 refs:2 ptrs: 4 -- -- -- -- -- -- --
    [   6 ]  ?   type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
    [   7 ]  ?   chunk(imap): 5 6 -- -- -- -- -- -- -- -- -- -- -- -- -- --


    ...
    [   4 ]  ?   [.,0] [..,0] [foo,1] -- -- -- -- --
    [   5 ]  ?   [.,1] [..,0] -- -- -- -- -- --
    [   6 ]  ?   type:dir size:1 refs:3 ptrs: 4 -- -- -- -- -- -- --
    [   7 ]  ?   type:dir size:1 refs:2 ptrs: 5 -- -- -- -- -- -- --
    [   8 ]  ?   chunk(imap): 6 7 -- -- -- -- -- -- -- -- -- -- -- -- -- --
    ```

9. The LFS simulator supports hard links as well. Run the following
to study how they work:
./lfs.py -L c,/foo:l,/foo,/bar:l,/foo,/goo -o -i.
What blocks are written out when a hard link is created? How is
this similar to just creating a new file, and how is it different? How
does the reference count field change as links are created?

    目录的inode和block要改变，记录新的条目  
    更新文件的inode的ref  
    imap执行最新的文件inode  

10. LFS makes many different policy decisions. We do not explore
many of them here – perhaps something left for the future – but here
is a simple one we do explore: the choice of inode number. First, run
./lfs.py -p c100 -n 10 -o -a s to show the usual behavior with the ”sequential” allocation policy, which tries to use free
inode numbers nearest to zero. Then, change to a ”random” policy
by running ./lfs.py -p c100 -n 10 -o -a r (the -p c100
flag ensures 100 percent of the random operations are file creations).
What on-disk differences does a random policy versus a sequential
policy result in? What does this say about the importance of choosing inode numbers in a real LFS?

    每个imap负责的是16个inode号，顺序分配时，全部10个文件可以由0号imap负责，所以每次更新写入都只会写入一个imap。如果是乱序，新文件的inode号超过15，就会启用新的imap，而根目录一定会更新，所以大概率创建一个文件会写入两个imap。因此尽量让文件和目录都在同一个imap中，这提示创建新目录时，也应该尽可能地分散到不同的imap中，和FFS的分组有类似的行为

11. One last thing we’ve been assuming is that the LFS simulator always updates the checkpoint region after each update. In the real
LFS, that isn’t the case: it is updated periodically to avoid long
seeks. Run ./lfs.py -N -i -o -s 1000 to see some operations and the intermediate and final states of the file system when
the checkpoint region isn’t forced to disk. What would happen if
the checkpoint region is never updated? What if it is updated periodically? Could you figure out how to recover the file system to the
latest state by rolling forward in the log?

    不更新CR，重新挂载文件系统时，无法找到最新的imap，还是初始化的imap，等效于数据全丢。  
    周期性更新，在周期中崩溃，再次挂载，只能找到上个周期的imap，最新的imap找不到。  
    前滚的策略，就是读最新的CR，找到新再继续看后续的更新是否合法，合法就更新它。
    比如下面这个，CR的最新imap能设计的最远block是3，所以从3开始看后面的段更新是否可也用，发现都是合法的，所以直接把CR更新为`13 -- -- -- ...`
    ```
    [   0 ]  ?   checkpoint: 3 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    [   1 ]  ?   [.,0] [..,0] -- -- -- -- -- --
    [   2 ]  ?   type:dir size:1 refs:2 ptrs: 1 -- -- -- -- -- -- --
    [   3 ]  ?   chunk(imap): 2 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    [   4 ]  ?   [.,0] [..,0] [tg4,1] -- -- -- -- --
    [   5 ]  ?   type:dir size:1 refs:2 ptrs: 4 -- -- -- -- -- -- --
    [   6 ]  ?   type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
    [   7 ]  ?   chunk(imap): 5 6 -- -- -- -- -- -- -- -- -- -- -- -- -- --
    [   8 ]  ?   type:reg size:6 refs:1 ptrs: -- -- -- -- -- -- -- --
    [   9 ]  ?   chunk(imap): 5 8 -- -- -- -- -- -- -- -- -- -- -- -- -- --
    [  10 ]  ?   [.,0] [..,0] [tg4,1] [lt0,2] -- -- -- --
    [  11 ]  ?   type:dir size:1 refs:2 ptrs: 10 -- -- -- -- -- -- --
    [  12 ]  ?   type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
    [  13 ]  ?   chunk(imap): 11 8 12 -- -- -- -- -- -- -- -- -- -- -- -- --
    ```
    当然细节是不懂的……
    ❓段不是缓冲在内存中的吗，那还没写到硬盘时就崩溃了，那些就丢失了？
