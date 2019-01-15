"""
Extracts k-mers of given size from the input DNA string.
"""

import sys

# Length of each k-mer.
pKmerLength = 20
# Jump size between consecutive k-mers.
pJumpSize = 1000
# Should we strip the FASTA header?
pHasFastaHeader = False

# All extracted k-mers with these characters will be excluded.
pExcludedCharacters = set("n")

# Input file path.
pInFilePath = "danio_rerio_chr1.fa"
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

def extract(data, kmerLength, jumpSize, excludedCharacters):
    end = len(data) - kmerLength + 1
    i = 0

    words = set()

    while i < end:
        perc = round(100.0 * i / end)

        sys.stdout.write("\r{0}/{1} ({2:.2f}%)".format(i, end, perc))
        sys.stdout.flush()

        word = data[i : i + kmerLength]
        isOK = True

        for excluded in excludedCharacters:
            if excluded in word:
                isOK = False
                break

        if isOK:
            words.add(word)

        i += jumpSize

    return words

def main():
    with open(pInFilePath, "r") as f:
        data = f.read().lower()

    sizeMB = len(data) / 1024.0 / 1024.0
    print "Read data from: {0}, size = {1:.2f} MB".format(pInFilePath, sizeMB)

    data = filterData(data, pHasFastaHeader)
    words = extract(data, pKmerLength, pJumpSize, pExcludedCharacters)

    with open(pOutFilePath, "w") as f:
        f.write("\n".join(words))
        print "\nDumped #words = {0} to: {1}".format(len(words), pOutFilePath)

if __name__ == "__main__":
    main()
