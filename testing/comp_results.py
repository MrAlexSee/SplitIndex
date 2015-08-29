#!/usr/bin/env python3

########
# Loads two files with results and looks for differences.
# Performs the necessary parsing and sorting.
########

import ast
import sys

errorQueries = set()

def matchAgainst(data1, data2):
    nErrors = 0

    for i1 in range(len(data1)):
        perc = 100.0 * (i1 + 1) / len(data1)

        sys.stdout.write("\r{0}/{1} ({2}%)".format(i1 + 1, len(data1), round(perc, 2)))
        sys.stdout.flush()

        query1 = data1[i1].split(":")[0]
        
        res1 = data1[i1].split(":")[1].strip()
        res1 = ast.literal_eval(res1)

        if not res1:
            continue

        matched = False
        appears = False

        for i2 in range(len(data2)):            
            query2 = data2[i2].split(":")[0]

            if query1 == query2:
                appears = True

                res2 = data2[i2].split(":")[1].strip()
                res2 = ast.literal_eval(res2)

                if set(res1) == set(res2):
                    matched = True
                    continue

        if (not matched) and (query1 not in errorQueries):
            nErrors += 1
            errorQueries.add(query1)
            print("\nERROR: {0} (appears = {1})".format(query1, appears))

    return nErrors

def main():
    if len(sys.argv) != 3:
        print("Usage: ./comp_results.py [input1] [input2]")
        sys.exit(1)

    with open(sys.argv[1], "r") as f:
        data1 = f.read()

    with open(sys.argv[2], "r") as f:
        data2 = f.read()

    data1 = [d for d in data1.split("\n") if d]
    data2 = [d for d in data2.split("\n") if d]

    print("\nPass 1/2")
    nErrors = matchAgainst(data1, data2)

    print("\nPass 2/2")
    nErrors += matchAgainst(data2, data1)

    print("\nTotal #errors = {0}".format(nErrors))

if __name__ == "__main__":
    main()
