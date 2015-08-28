#!/usr/bin/env python3

########
# Divides the continuous genome string into k-mers.
########

# Length of the k-mer.
kmer = 20
# Jump size between consecutive k-mers.
jump = 500

# Should we strip the FASTA header?
hasFastaHeader = True

assert(kmer > 0 and jump > 0)
import sys

if len(sys.argv) != 3:
    print("Usage: ./dna_to_dict.py [input] [output]")
    sys.exit(1)

with open(sys.argv[1], "r") as f:
    data = f.read().lower()

sizeMB = len(data) / 1024.0 / 1024.0
print("Read the data, #chars = {0}, {1}MB".format(len(data), round(sizeMB, 2)))

if hasFastaHeader:
    # We strip the FASTA header.
    first = len(data.split("\n")[0]) + 1
    data = data[first : ]

dnaChars = set(["a", "c", "g", "n", "t"])
data = [c for c in data if c in dnaChars]

sizeMB = len(data) / 1024.0 / 1024.0
print("Filtered the data, #chars = {0}, {1}MB".format(len(data), round(sizeMB, 2)))

nWords = 0
i = 0

with open(sys.argv[2], "w") as f: # Truncates the file.
    fin = len(data) - kmer + 1

    while i < fin:
        perc = round(100.0 * (i + 1) / len(data))

        sys.stdout.write("\r{0}/{1} ({2}%)".format(i + 1, fin, perc))
        sys.stdout.flush()

        word = data[i : i + kmer]
        f.write("".join(word))
        f.write("\n")

        i += jump
        nWords += 1

print("\nWritten {0} words to: {1}".format(nWords, sys.argv[2]))
