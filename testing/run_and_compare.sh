#!/bin/bash

set -e

dictPath="/home/alex/Documents/Textual/English dict/english-dict_filtered.txt"
queriesPath="english_typos.txt"

indexOutPath="search_ham.dat" # Must be the same as the output path of the index (params.hpp : outFilePath)
naiveOutPath="out_naive.dat"

# Make sure that both programs are set for the same k and min word length value.

# First we run the naive hamming.
./naive_hamming.py "$dictPath" "$queriesPath" "$naiveOutPath"

# Followed by our index-based search.
../build/main "$dictPath" "$queriesPath"

# And we compare the results for errors.
./comp_results.py "$naiveOutPath" "$indexOutPath"

rm "$indexOutPath" "$naiveOutPath"
echo "All finished"
