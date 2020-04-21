


1. [变量] fork之后，共享的内存写时复制，父子进程互不影响

2. [描述符 & 写入] 子进程会复制已打开的描述符，可以访问，都指向全局的文件表。写入交叉执行，顺序不定。  
   write本身是"原子"的吗？3.14之后是。`man 2 write` BUGS一节有说明。所以写入超过4096也没有问题

3. [同步] 不用wait就要用共享内存的锁？全局的信号量？当前的知识而言，好像是提示用pipe？

4. [exec family] `l` 多变量的方式给参数，`p` 从path环境变量中找执行文件，`e` 传入环境变量

5. [wait] `wait`成功返回终止的进程id，出错返回-1. 子进程里wait返回-1，因为已经知道它的进程结构中没有子进程信息

6. [wait pid] `wait(&status)` 就是 `waitpid(-1, &status, 0)`，在**不想阻塞、需要定制等待对象**时使用`waitpid`
   pid|等谁呢？
   --|--
   < -1 | group id中的任意一个子进程
   -1 | 任意子进程
   0 | 调用`wait`的group中的任意一个子进程
   > 0 | 指定pid的子进程

   option | 含义
   --|--
   ⭐WNOHANG | no hang 别挂起，没有子进程返回就立即返回，返回值为0
   WUNTRACED | untraced 如果子进程被stop也要返回
   WCONTINUED | continued 如果子进程继续执行也要返回

7. [描述符close] 子进程关闭stdout描述符之后写入，`write`返回-1，`printf`什么都没写入


8. [pipe] 用pipe连接两个子进程的stdout、stdout  
   超纲了呀  
   - `dup(int oldfd)` 占用一个没有使用整数，指向`oldfd`所指向的文件表项
   - `dup2(int oldfd, int newfd)` 占用`newfd`，指向`oldfd`所指向的文件表项，如果`newfd`已经使用，会先关闭
