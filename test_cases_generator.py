import sys
from random import randint


try:
    # get map size, number of targets and number of attacks
    N, T, B = int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3])
except IndexError:
    print('\nusage:\n  python testcase_builder.py <MAP_SIZE> <NUMBER_TARGETS> <NUMBER_ATTACKS> > output.txt')
    print('This is a little python script to generate random test cases for the bombing fields problem.')
    print('Exiting.\n')
    exit(0)

# start printing the test case
print(N)
print(T)

# build targets
for _ in range(T):
    x, y, v = randint(0, N-1), randint(0, N-1), randint(-N, N)
    print('%d %d %d' % (x, y, v))

print(B)

# build attacks
for _ in range(B):
    x, y, r, p = randint(0, N-1), randint(0, N-1), randint(0, N/4), randint(1, 10)
print('%d %d %d %d' % (x, y, r, p))