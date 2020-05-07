1. Stat
```c
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
int stat(const char *pathname, struct stat *statbuf);  //从文件名获取
int fstat(int fd, struct stat *statbuf);               //从文件描述符获取
int lstat(const char *pathname, struct stat *statbuf); //如果是软连接，读取软连接本身

#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>
int fstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags);
// 通用的接口，可以用AT_* flags达成上面三个调用的效果
```

~~目录的`link count`就是当前inode的引用数量，和目录中的entry数量无关~~

目录的link_count就是其中目录的数量(包含.和..), 之前不变是WSL的行为，可能没处理好
