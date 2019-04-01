import sys

lines = [line.split() for line in open(sys.argv[1]).read().split('\n') if line]

branches = {}
for x in lines:
    if x[0] in branches:
        branches[x[0]] += x[1]
    else:
        branches[x[0]] = x[1]

for b in branches:
    print(b, branches[b])
