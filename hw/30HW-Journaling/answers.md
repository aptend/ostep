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
没有关于日志的模拟好可惜
```

1. vsfs一节做过
2. readme中的例题，inode bitmap中文件`/m`的占用丢了
3. -S3: 文件的目录`/m`的硬链接数量不对，没人连接它  
   -S19: 目录`/g`的ref count不对，目录最少都有`.`和`..`两个  
   这个可以根据文件系统内部的约束进行自动修改
4. -S5: 目录`/g`的内容错乱，`..`的名字变成了`y`  -可检测，可修复  
   -S38: 文件名`/w/p`变成了`/w/b`  -不可检测  
   -S642: 目录重名了，两个`/w`  -可检测，不可修复，不知道该是什么名字

5. -S6: inode12 被目录占用，但是没有目录引用  
   -S13: inode10 被文件占用，但是没有目录引用  
   放到`lost+found`里
6. -S9: inode13的类型和类容不匹配，应该比对inode中的其他信息和datablock的内容，比如连接，大小等等，选择一个改动少的，比如这里只用把格式变为文件就匹配上了
7. -S15: 根目录变成了文件，可以修复，根目录的位置众所周知。如果不修复，全部数据都不能正常读取
8. -S10: `/w`的父目录指向了未分配的inode3，现在不知道这个目录被创建在哪里了。恢复时检查其他目录的内容，看谁有`w`的条目
9. -S16: 文件inode13指向了未分配的block，只能删除  
   -S20: 目录inode8指向了未分配的block，这个修复尝试找没有被引用的目录的data block， ref count对应并且`.` 指向了inode8

