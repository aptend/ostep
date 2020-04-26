1. First let’s use a tiny address space to translate some addresses. Here’s a simple set of parameters with a few different random seeds; can you translate the addresses?
   - `-a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 0`
   - `-a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 1`
   - `-a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 2`
2. Now, let’s see if we understand this tiny address space we’ve constructed (using the parameters from the question above). What is the highest legal virtual address in segment 0? What about the lowest legal virtual address in segment 1? What are the lowest and highest illegal addresses in this entire address space? Finally, how would you run segmentation.py with the -A flag to test if you are right?
3. Let’s say we have a tiny 16-byte address space in a 128-byte physical memory. What base and bounds would you set up so as to get the simulator to generate the following translation results for the specified address stream: valid, valid, violation, ..., violation, valid, valid? Assume the following parameters:
    - `-a 16 -p 128 -A 0,1,2,3,4,5,6,7,8,9,10,11,12,13, 14,15  --b ? --l ? --B ? --L ?`
4. Assume we want to generate a problem where roughly 90% of the randomly-generated virtual addresses are valid (not segmentation violations). How should you configure the simulator to do so? Which parameters are important to getting this outcome? 
5. Can you run the simulator such that no virtual addresses are valid? How?

--- 

1. 这作业我做了，但是没有带
2. s0时，段0最高合法地址19，段1最低合法108。整个虚拟地址空间的最高合法127， 最低合法0:
    ```
    -a128 -p512 -b0 -l20 -B512 -L20 -s0 -A 0,127 -c
    -a128 -p512 -b0 -l20 -B512 -L20 -s0 -A 19,108 -c
    -a128 -p512 -b0 -l20 -B512 -L20 -s0 -A 20,107 -c
    ```
3. `-b0 -l2 -B16 -L2`

4. 限制很重要,128*0.9 = 115 参数`-a128 -p512 -b0 -l51 -B512 -L64`就可以。因为这里分为两个部分，每个部分的最大限制为64

5. 限制为0
