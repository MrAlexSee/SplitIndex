#!/bin/bash

inputDir="data"
outFile="res.txt"

maxLF=3.0
nIter=1

# All index types.
for iType in k1;
do
    # All hash types.
    for hType in city farm farsh fnv1 fnv1a murmur3 sdbm spookyv2 superfast xxhash;
    do
        # Selected dictionaries.
        for dictFile in dict_foster dict_iamerican_insane;
        do
            ./build/split_index --index-type $iType --hash-type $hType --max-load-factor $maxLF --iter $nIter -i ${inputDir}/${dictFile}.txt -I ${inputDir}/queries_misspellings.txt -o $outFile -d
        done
    done
done
