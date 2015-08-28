#!/usr/bin/env python3

########
# Performs an approximate search in a dictionary
# using a Hamming distance and a naive algorithm.
########

import sys

minWordLength = 4
maxNQueries   = -1 # -1 to turn off
hamK          = 1  # Select the number of errors here.

def calcHammingK(str1, str2, k):
    assert len(str1) == len(str2)
    nErrors = 0

    for i in range(len(str1)):
        if str1[i] != str2[i]:
            nErrors += 1

            if nErrors > k:
                return False
    return True

def dumpToFile(filePath, results):
    with open(filePath, "w") as f: # Truncates
        for r in results:
            f.write("{0}: {1}\n".format(r[0], r[1]))

def search(data, query):
    res = []

    for word in data:
        if len(word) < minWordLength:
            continue

        if len(word) == len(query):
            if calcHammingK(word, query, hamK):
                res.append(word)
    return res

def main():
    if len(sys.argv) < 3 or len(sys.argv) > 4:
        print("Usage: ./naive_hamming.py [input] [queries] [opt: output]")
        sys.exit(1)

    with open(sys.argv[1], "r") as f:
        data = f.read().lower()

    with open(sys.argv[2], "r") as f:
        queries = f.read().lower()

    data = [d for d in data.split("\n") if d]
    queries = [q for q in queries.split("\n") if q]

    if maxNQueries != -1:
        queries = queries[0 : maxNQueries]
    
    results = []

    print("Calculating naive Hamming for k = {0}".format(hamK))

    for i in range(len(queries)):
        perc = round(100.0 * (i + 1) / len(queries))
     
        sys.stdout.write("\r{0}/{1} ({2}%)".format(i + 1, len(queries), perc))
        sys.stdout.flush()

        res = search(data, queries[i])
        results.append([queries[i], res])

    if len(sys.argv) == 4:
        dumpToFile(sys.argv[3], results)
        print("\nDumped to file: " + sys.argv[3])
    else:
        for r in results:
            print("{0}: {1}".format(r[0], r[1]))

if __name__ == "__main__":
    main()
