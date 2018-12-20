"""
Determines the optimal counts of most frequent 2-, 3-, and 4-grams from the input text.
Optimal = providing highest compression for character substitution coding.
"""

import collections
import itertools

# Input dictionary file path.
pInFilePath = "dict.txt"

# Total number (sum) of 2-,3-,4-grams.
pTotalNQgrams = 5

def readWords(inFilePath):
    with open(inFilePath, "r") as f:
        text = f.read()

    lines = [l for l in text.split("\n") if l]
    sizeMB = len("".join(lines)) / 1000.0 / 1000.0
    
    print "Read data from: {0}, size = {1:.2f} MB".format(inFilePath, sizeMB)
    print "#words = {0}".format(len(lines))

    return lines

def extractQgramsOrderedByFrequency(words):
    ret = []

    for qgramSize in [2, 3, 4]:
        print "Extracting {0}-grams...".format(qgramSize)
        counter = collections.Counter()

        for word in words:
            end = len(word) - qgramSize + 1

            for i in xrange(end):
                assert i + qgramSize <= len(word)
              
                qgram = word[i : i + qgramSize]
                counter[qgram] += 1

        ret += [[key for key in counter]]

    print "Extracted #q-grams: {0} (2-,3-,4-)".format([len(q) for q in ret])
    return ret

# This simply uses one character from outside the text for substitution.
# This is because we only care about the resulting word size here.
def fakeEncode(word, qgrams, qgramSize):
    assert len([q for q in qgrams if len(q) == qgramSize]) == len(qgrams)

    qgramEnd = len(word) - qgramSize + 1
    ret = ""

    i = 0
    while i < len(word):
        if i < qgramEnd:
            if word[i : i + qgramSize] in qgrams:
                ret += "*"
                i += qgramSize
            else:
                ret += word[i]
                i += 1
        else:
            ret += word[i]
            i += 1

    return ret

def compress(word, cur2Grams, cur3Grams, cur4Grams):
    enc4 = fakeEncode(word, cur4Grams, 4)
    enc3 = fakeEncode(enc4, cur3Grams, 3)
    return fakeEncode(enc3, cur2Grams, 2)

def calcCompressionRatio(words, qgrams, qgramCounts):
    assert len(qgrams) == len(qgramCounts) == 3
    print "Calculating compression ratio for: {0} (2-,3-,4-)".format(qgramCounts)
    
    cur2Grams = set(qgrams[0][ : qgramCounts[0]])
    cur3Grams = set(qgrams[1][ : qgramCounts[1]])
    cur4Grams = set(qgrams[2][ : qgramCounts[2]])

    compressedWords = []

    for word in words:
        compressedWords += [compress(word, cur2Grams, cur3Grams, cur4Grams)]    

    uncompressedSize = len("".join(words))
    compressedSize = len("".join(compressedWords))

    return float(uncompressedSize) / compressedSize

def calcOptimalCompression(words, totalNQgrams):
    countsList = [i for i in xrange(totalNQgrams + 1)]
    combinations = [counts for counts in itertools.product(countsList, countsList, countsList) if sum(counts) == totalNQgrams]
    
    print "Identified #combinations = {0} with sum = {1}".format(len(combinations), totalNQgrams)
    qgrams = extractQgramsOrderedByFrequency(words)

    results = []

    for combination in combinations:
        results += [(combination, calcCompressionRatio(words, qgrams, combination))]

    # We return the permutation with the highest compression score.
    return sorted(results, key = lambda t: t[1], reverse = True)[0]

def main():
    words = readWords(pInFilePath)
    counts, ratio = calcOptimalCompression(words, pTotalNQgrams)

    print "Optimal counts = {0} (2-,3-,4-)".format(counts)
    print "Compression ratio = {0}".format(ratio)

if __name__ == "__main__":
    main()
