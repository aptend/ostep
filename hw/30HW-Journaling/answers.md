1. First, run fsck.py -D; this flag turns off any corruption, and thus
you can use it to generate a random file system, and see if you can
determine which files and directories are in there. So, go ahead and
do that! Use the -p flag to see if you were right. Try this for a few
different randomly-generated file systems by setting the seed (-s)
to different values, like 1, 2, and 3.
2. Now, let’s introduce a corruption. Run fsck.py -S 1 to start.
Can you see what inconsistency is introduced? How would you fix
it in a real file system repair tool? Use -c to check if you were right.
3. Change the seed to -S 3 or -S 19; which inconsistency do you
see? Use -c to check your answer. What is different in these two
cases?
4. Change the seed to -S 5; which inconsistency do you see? How
hard would it be to fix this problem in an automatic way? Use -c to
check your answer. Then, introduce a similar inconsistency with -S
38; is this harder/possible to detect? Finally, use -S 642; is this
inconsistency detectable? If so, how would you fix the file system?
5. Change the seed to -S 6 or -S 13; which inconsistency do you
see? Use -c to check your answer. What is the difference across
these two cases? What should the repair tool do when encountering
such a situation?
6. Change the seed to -S 9; which inconsistency do you see? Use -c
to check your answer. Which piece of information should a checkand-repair tool trust in this case?
7. Change the seed to -S 15; which inconsistency do you see? Use
-c to check your answer. What can a repair tool do in this case? If
no repair is possible, how much data is lost?
8. Change the seed to -S 10; which inconsistency do you see? Use
-c to check your answer. Is there redundancy in the file system
structure here that can help a repair?
9. Change the seed to -S 16 and -S 20; which inconsistency do you
see? Use -c to check your answer. How should the repair tool fix
the problem?

---

```
Summary of files, directories::
  Files:       ['/t', '/m', '/w/p']
  Directories: ['/', '/g', '/w']
```

1. 这个不是在vsfs一节做过了？
2. readme中的例题，inode bitmap中文件`/m`的占用丢了
3. -S3: 文件的目录`/m`的硬链接数量不对，没人连接它  
   -S19: 目录`/g`的ref count不对，目录最少都有`.`和`..`两个
4. -S5: 目录`/g`的内容错乱，`..`的名字变成了`y`  
   -S38: 文件名`/w/p`变成了`/w/b`  
   -S642: 