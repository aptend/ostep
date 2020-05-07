1. Run the simulator with some different random seeds (say 17, 18, 19,
20), and see if you can figure out which operations must have taken
place between each state change.
2. Now do the same, using different random seeds (say 21, 22, 23,
24), except run with the -r flag, thus making you guess the state
change while being shown the operation. What can you conclude
about the inode and data-block allocation algorithms, in terms of
which blocks they prefer to allocate?
3. Now reduce the number of data blocks in the file system, to very
low numbers (say two), and run the simulator for a hundred or so
requests. What types of files end up in the file system in this highlyconstrained layout? What types of operations would fail?
4. Now do the same, but with inodes. With very few inodes, what
types of operations can succeed? Which will usually fail? What is
the final state of the file system likely to be?

---

1. `-s 17`
   ```
    inode bitmap  10000000
    inodes        [d a:0 r:2] [] [] [] [] [] [] []
    data bitmap   10000000
    data          [(.,0) (..,0)] [] [] [] [] [] [] []

    mkdir('u')

    inode bitmap  11000000
    inodes        [d a:0 r:3] [d a:1 r:2] [] [] [] [] [] [] 
    data bitmap   11000000
    data          [(.,0) (..,0) (u,1)] [(.,1) (..,0)] [] [] [] [] [] []

    creat('a')

    inode bitmap  11100000
    inodes        [d a:0 r:3] [d a:1 r:2] [f a:-1 r:1] [] [] [] [] [] 
    data bitmap   11000000
    data          [(.,0) (..,0) (u,1) (a,2)] [(.,1) (..,0)] [] [] [] [] [] []

    unlink('a')

    inode bitmap  11000000
    inodes        [d a:0 r:3] [d a:1 r:2] [] [] [] [] [] []
    data bitmap   11000000
    data          [(.,0) (..,0) (u,1)] [(.,1) (..,0)] [] [] [] [] [] []

    mkdir('z')

    inode bitmap  11100000
    inodes        [d a:0 r:4] [d a:1 r:2] [d a:2 r:2] [] [] [] [] []
    data bitmap   11100000
    data          [(.,0) (..,0) (u,1) (z,2)] [(.,1) (..,0)] [(.,2) (..,0)] [] [] [] [] []

    mkdir('s')

    inode bitmap  11110000
    inodes        [d a:0 r:5] [d a:1 r:2] [d a:2 r:2] [d a:3 r:2] [] [] [] [] 
    data bitmap   11110000
    data          [(.,0) (..,0) (u,1) (z,2) (s,3)] [(.,1) (..,0)] [(.,2) (..,0)] [(.,3) (..,0)] [] [] [] [] 

    creat('z/x')

    inode bitmap  11111000
    inodes        [d a:0 r:5] [d a:1 r:2] [d a:2 r:2] [d a:3 r:2] [f a:-1 r:1] [] [] []
    data bitmap   11110000
    data          [(.,0) (..,0) (u,1) (z,2) (s,3)] [(.,1) (..,0)] [(.,2) (..,0) (x,4)] [(.,3) (..,0)] [] [] [] []

    link('z/x', 'u/b')

    inode bitmap  11111000
    inodes        [d a:0 r:5] [d a:1 r:2] [d a:2 r:2] [d a:3 r:2] [f a:-1 r:2] [] [] []
    data bitmap   11110000
    data          [(.,0) (..,0) (u,1) (z,2) (s,3)] [(.,1) (..,0) (b,4)] [(.,2) (..,0) (x,4)] [(.,3) (..,0)] [] [] [] []

    unlink('u/b')

    inode bitmap  11111000
    inodes        [d a:0 r:5] [d a:1 r:2] [d a:2 r:2] [d a:3 r:2] [f a:-1 r:1] [] [] [] 
    data bitmap   11110000
    data          [(.,0) (..,0) (u,1) (z,2) (s,3)] [(.,1) (..,0)] [(.,2) (..,0) (x,4)] [(.,3) (..,0)] [] [] [] []

    write('z/x')

    inode bitmap  11111000
    inodes        [d a:0 r:5] [d a:1 r:2] [d a:2 r:2] [d a:3 r:2] [f a:4 r:1] [] [] []
    data bitmap   11111000
    data          [(.,0) (..,0) (u,1) (z,2) (s,3)] [(.,1) (..,0)] [(.,2) (..,0) (x,4)] [(.,3) (..,0)] [v] [] [] []

    creat('u/b')

    inode bitmap  11111100
    inodes        [d a:0 r:5] [d a:1 r:2] [d a:2 r:2] [d a:3 r:2] [f a:4 r:1] [f a:-1 r:1] [] []
    data bitmap   11111000
    data          [(.,0) (..,0) (u,1) (z,2) (s,3)] [(.,1) (..,0) (b,5)] [(.,2) (..,0) (x,4)] [(.,3) (..,0)] [v] [] [] []
   ```

2. 按顺序分配inode和data空间，有空洞就填空洞

3. 只有两个data block，根目录占用一个，要么创建文件后写入一个文件内容，要么创建一个新的目录。占满之后只有creat\unlink可以成功，write一定失败。最终大概率是两个目录和空文件。
4. 只有两个inode空间，根目录占一个，就最多创建一个目录和新建一个文件。最终大概率文件被删除，剩下的是目录。文件不能有的。inode就是文件系统中目录和文件的上限
