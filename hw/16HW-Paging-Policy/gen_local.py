import random
import sys

# 20-80 work load

SEED = 1
PAGE_SIZE = 100
random.seed(SEED)
pages = list(range(PAGE_SIZE))
random.shuffle(pages)
hot = pages[:int(PAGE_SIZE * 0.2)]
warm = pages[int(PAGE_SIZE * 0.2):]

if len(sys.argv) < 2:
    print("specify the number of addresses to generate")
    sys.exit(0)

for i in range(int(sys.argv[1])):
    if random.random() < 0.8:
        print(random.choice(hot))
    else:
        print(random.choice(warm))
