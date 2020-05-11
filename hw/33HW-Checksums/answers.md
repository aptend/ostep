1. First just run checksum.py with no arguments. Compute the additive, XOR-based, and Fletcher checksums. Use -c to check your
answers.
2. Now do the same, but vary the seed (-s) to different values.
3. Sometimes the additive and XOR-based checksums produce the
same checksum (e.g., if the data value is all zeroes). Can you pass
in a 4-byte data value (using the -D flag, e.g., -D a,b,c,d) that
does not contain only zeroes and leads the additive and XOR-based
checksum having the same value? In general, when does this occur? Check that you are correct with the -c flag.
    
    加法和异或的结果相同，不进位加法本身就是异或。`-D 1,2,4,8`

4. Now pass in a 4-byte value that you know will produce a different
checksum values for additive and XOR. In general, when does this
occur?

    存在进位。`-D 1,1,1,1`

5. Use the simulator to compute checksums twice (once each for a different set of numbers). The two number strings should be different
(e.g., -D a1,b1,c1,d1 the first time and -D a2,b2,c2,d2 the
second) but should produce the same additive checksum. In general, when will the additive checksum be the same, even though the
data values are different? Check your specific answer with the -c
flag.

    加法溢出，`-D 1,0,0,0` 和 `-D 255,1,1,0`


6. Now do the same for the XOR checksum.

    每位的1出现的奇偶性相同，`-D 1,2,3,4`把最低位增加两个1变为`-D 1,3,3,5`

7. Now let’s look at a specific set of data values. The first is: -D
1,2,3,4. What will the different checksums (additive, XOR, Fletcher)
be for this data? Now compare it to computing these checksums
over -D 4,3,2,1. What do you notice about these three checksums? How does Fletcher compare to the other two? How is Fletcher
generally “better” than something like the simple additive checksum?

    ```
    1234
    Add:             10       (0b00001010)
    Xor:              4       (0b00000100)
    Fletcher(a,b):   10, 20   (0b00001010,0b00010100)

    4321
    Add:             10       (0b00001010)
    Xor:              4       (0b00000100)
    Fletcher(a,b):   10, 30   (0b00001010,0b00011110)
    ```

    Fletcher是顺序相关的，Xor和Add都顺序无关

8. No checksum is perfect. Given a particular input of your choosing,
can you find other data values that lead to the same Fletcher checksum? When, in general, does this occur? Start with a simple data
string (e.g., -D 0,1,2,3) and see if you can replace one of those
numbers but end up with the same Fletcher checksum. As always,
use -c to check your answers.

    Fletcher的第一个结果是加法，只不过取模255。第二个是最后两个累积和对255取模。所以要相等，需要最后两个数的顺序一致，并且满足加法溢出。
    `-D 0,1,2,3` 变为 `-D 255,1,2,3`


---

1. Write a short C program (called check-xor.c) that computes an
XOR-based checksum over an input file, and prints the checksum as
output. Use a 8-bit unsigned char to store the (one byte) checksum.
Make some test files to see if it works as expected.
2. Now write a short C program (called check-fletcher.c) that
computes the Fletcher checksum over an input file. Once again,
test your program to see if it works.
3. Now compare the performance of both: is one faster than the other?
How does performance change as the size of the input file changes?
Use internal calls to gettimeofday to time the programs. Which
should you use if you care about performance? About checking
ability?

    性能找xor，校验能力要fletcher。
    fletcher计算两个，还要对255取模。

4. Read about the 16-bit CRC and then implement it. Test it on a number of different inputs to ensure that it works. How is its performance as compared to the simple XOR and Fletcher? How about
its checking ability?
5. Now build a tool (create-csum.c) that computes a single-byte
checksum for every 4KB block of a file, and records the results in
an output file (specified on the command line). Build a related tool
(check-csum.c) that reads a file, computes the checksums over
each block, and compares the results to the stored checksums stored
in another file. If there is a problem, the program should print that
the file has been corrupted. Test the program by manually corrupting the file.
