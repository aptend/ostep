1. 正常退出， `free`的文档：If ptr is NULL, no operation is performed.
2. `[Inferior 1 (process 437) exited normally]`
3. ```
    ==444== error calling PR_SET_PTRACER, vgdb might block
    ==444== 
    ==444== HEAP SUMMARY:
    ==444==     in use at exit: 0 bytes in 0 blocks
    ==444==   total heap usage: 0 allocs, 0 frees, 0 bytes allocated
    ==444== 
    ==444== All heap blocks were freed -- no leaks are possible
    ```
4. 程序和gdb正常退出， valgrind显示内存泄漏
    ```
    ==644== HEAP SUMMARY:
    ==644==     in use at exit: 4 bytes in 1 blocks
    ==644==   total heap usage: 1 allocs, 0 frees, 4 bytes allocated
    ==644== 
    ==644== 4 bytes in 1 blocks are definitely lost in loss record 1 of 1
    ==644==    at 0x4C31B25: calloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==644==    by 0x108660: main (in /mnt/d/C2_Repos/ostep/hw/08HW-Memory-API/forget)
    ==644== 
    ==644== LEAK SUMMARY:
    ==644==    definitely lost: 4 bytes in 1 blocks
    ==644==    indirectly lost: 0 bytes in 0 blocks
    ==644==      possibly lost: 0 bytes in 0 blocks
    ==644==    still reachable: 0 bytes in 0 blocks
    ==644==         suppressed: 0 bytes in 0 blocks
    ```

5. 程序正常退出，但是valgrind显示alloc越界非法写入:
    ```
    ==1592== Invalid write of size 4
    ==1592==    at 0x10866A: main (5.c:6)
    ==1592==  Address 0x522d1d0 is 0 bytes after a block of size 400 alloc'd
    ==1592==    at 0x4C2FB0F: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==1592==    by 0x10865B: main (5.c:5)
    ```
6. 程序正常运行，valgrind显示free之后的非法写入

7. valgrind
    ```
    Invalid write of size 4
    ==1043==    at 0x1086B5: main (in /mnt/d/C2_Repos/ostep/hw/08HW-Memory-API/a.out)
    ==1043==  Address 0x522d040 is 0 bytes inside a block of size 20 free'd
    ==1043==    at 0x4C30D3B: free (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==1043==    by 0x1086B0: main (in /mnt/d/C2_Repos/ostep/hw/08HW-Memory-API/a.out)
    ==1043==  Block was alloc'd at
    ==1043==    at 0x4C31B25: calloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==1043==    by 0x1086A0: main (in /mnt/d/C2_Repos/ostep/hw/08HW-Memory-API/a.out)
    ```

7. 程序：`free(): invalid pointer`, valgrind输出：
    ```
     Invalid free() / delete / delete[] / realloc()
    ==985==    at 0x4C30D3B: free (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==985==    by 0x1086B4: main (in /mnt/d/C2_Repos/ostep/hw/08HW-Memory-API/a.out)
    ==985==  Address 0x522d048 is 8 bytes inside a block of size 20 alloc'd
    ==985==    at 0x4C31B25: calloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==985==    by 0x1086A0: main (in /mnt/d/C2_Repos/ostep/hw/08HW-Memory-API/a.out)
    ```
