#!/usr/bin/env python3

########
# Transforms a dictionary of string to lower case and removes the lines
# which contain characters that are not part of the alphabet.
########

import sys

alphabet = "abcdefghijklmnopqrstuvwxyz\'"
aSet = set(alphabet)

sep = "\n" # A separator for input words.

if len(sys.argv) != 3:
    print("Usage: ./filter.py [input] [output]")
    sys.exit(1)

print("Filtering for alphabet = <{0}> (length = {1})".format(alphabet, len(alphabet)))

with open(sys.argv[1], "r") as f:
    data = f.read()

words = [w for w in data.split(sep) if w]

nRemoved = 0
out = []

with open(sys.argv[2], "w") as f: # Truncates the file.
    for word in words:
        word = word.lower()
        
        ok = True

        for c in word:
            if c not in aSet:
                ok = False
                break

        if ok:
            f.write(word + "\n")
        else:
            nRemoved += 1

print("Removed {0} lines, written output to: {1}".format(nRemoved, sys.argv[2]))
