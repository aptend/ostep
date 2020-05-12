1. A first question for your trace analysis: using the timestamps found
in the first column, determine the period of time the traces were
taken from. How long is the period? What day/week/month/year
was it? (does this match the hint given in the file name?) Hint: Use
the tools head -1 and tail -1 to extract the first and last lines of
the file, and do the calculation.

    ```
    P1
    --------------------
    start time       2002-10-17 01:00:00.773984
    end time         2002-10-17 01:59:59.541974
    delta time       0:59:58.767990
    ```

2. Now, let’s do some operation counts. How many of each type of operation occur in the trace? Sort these by frequency; which operation
is most frequent? Does NFS live up to its reputation?

    ```
    P2
    --------------------
    [('getattr', 1610395),
    ('read', 1043752),
    ('write', 619819),
    ('lookup', 131453),
    ('access', 27699),
    ('setattr', 19485),
    ('remove', 11640),
    ('create', 9924),
    ('fsstat', 9135),
    ('link', 4290),
    ('readdirp', 2297),
    ('fsinfo', 1321),
    ('readdir', 918),
    ('rename', 501),
    ('readlink', 439),
    ('pathconf', 187),
    ('symlink', 136),
    ('mkdir', 36),
    ('rmdir', 14),
    ('mknod', 4)]
    ```

    getattr 最多

3. Now let’s look at some particular operations in more detail. For
example, the GETATTR request returns a lot of information about
files, including which user ID the request is being performed for,
the size of the file, and so forth. Make a distribution of file sizes
accessed within the trace; what is the average file size? Also, how
many different users access files in the trace? Do a few users dominate traffic, or is it more spread out? What other interesting information is found within GETATTR replies?



4. You can also look at requests to a given file and determine how
files are being accessed. For example, is a given file being read or
written sequentially? Or randomly? Look at the details of READ
and WRITE requests/replies to compute the answer.
5. Traffic comes from many machines and goes to one server (in this
trace). Compute a traffic matrix, which shows how many different
clients there are in the trace, and how many requests/replies go to
each. Do a few machines dominate, or is it more evenly balanced?
6. The timing information, and the per-request/reply unique ID, should
allow you to compute the latency for a given request. Compute the
latencies of all request/reply pairs, and plot them as a distribution.
What is the average? Maximum? Minimum?
7. Sometimes requests are retried, as the request or its reply could be
lost or dropped. Can you find any evidence of such retrying in the
trace sample?
8. There are many other questions you could answer through more
analysis. What questions do you think are important? Suggest
them to us, and perhaps we’ll add them here!
