"""
Extracts k-mers of given size from the input DNA string.
"""

import sys

# Length of each k-mer.
pKmerLength = 20
# Jump size between consecutive k-mers.
pJumpSize = 500
# Should we strip the FASTA header?
pHasFastaHeader = True

# Input file path.
pInFilePath = "fruitfly.fa"
# Output file path.
pOutFilePath = "dict.txt"

def filterData(data, hasFastaHeader):
    print "Filtering data..."

    if hasFastaHeader:
        iFirstChar = len(data.split("\n")[0]) + 1
        data = data[iFirstChar : ]

    dnaChars = set("acgtn")
    
    data = [c for c in data if c in dnaChars]
    data = "".join(data)

    sizeMB = len(data) / 1000.0 / 1000.0
    print "Filtered data, #chars = {0}, {1:.2f} MB".format(len(data), sizeMB)

    return data

def processAndDump(data, kmerLength, jumpSize, outFile):
    end = len(data) - kmerLength + 1
    i = 0

    nWords = 0

    with open(outFile, "w") as f:
        while i < end:
            perc = round(100.0 * i / end)

            sys.stdout.write("\r{0}/{1} ({2:.2f}%)".format(i, end, perc))
            sys.stdout.flush()

            word = data[i : i + kmerLength]
            f.write(word + "\n")

            i += jumpSize
            nWords += 1

    print "\nDumped #words = {0} to: {1}".format(nWords, outFile)

def main():
    with open(pInFilePath, "r") as f:
        data = f.read().lower()

    sizeMB = len(data) / 1024.0 / 1024.0
    print "Read data from: {0}, size = {1:.2f} MB".format(pInFilePath, sizeMB)

    data = filterData(data, pHasFastaHeader)
    processAndDump(data, pKmerLength, pJumpSize, pOutFilePath)

if __name__ == "__main__":
    main()
