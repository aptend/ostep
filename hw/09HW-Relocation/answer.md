1. Run with seeds 1, 2, and 3, and compute whether each virtual address generated by the process is in or out of bounds. If in bounds,compute the translation.
2. Run with these flags: -s 0 -n 10. What value do you have set -l (the bounds register) to in order to ensure that all the generated virtual addresses are within bounds?
3. Run with these flags: -s 1 -n 10 -l 100. What is the maximum value that base can be set to, such that the address space still fits into physical memory in its entirety?
4. Run some of the same problems above, but with larger address spaces (-a) and physical memories (-p).
5. What fraction of randomly-generated virtual addresses are valid, as a function of the value of the bounds register? Make a graph from running with different random seeds, with limit values ranging from 0 up to the maximum size of the address space.

---

1. 指令地址满足 < 290 | < 500 | <316 就是合法的
2. 最大的地址为929， 所以-l930
3. 16K-857-1
4. a增大，随机出的测试地址范围更大，limit没变，所以出现更多的非法地址; p增大， 随机出的基地址更大，但是对是否非法的判断没有影响
5. 地址合法的概率 `p = [l/a for for l in range(0, a)]`

