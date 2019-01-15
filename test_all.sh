#!/bin/bash

inputDir="data"
outFile="res.txt"

maxLF=3.0
nIter=1

# All index types.
for iType in k1 k1comp k1comptriple k2 k3;
do
    # All hash types.
    for hType in city farm farsh fnv1 fnv1a murmur3 sdbm spookyv2 superfast xxhash;
    do
        # English dictionaries.
        for dictFile in dict_english_foster dict_english_iamerican_insane;
        do
            ./build/split_index --index-type $iType --hash-type $hType --max-load-factor $maxLF --iter $nIter -i ${inputDir}/${dictFile}.txt -I ${inputDir}/queries_english_misspellings.txt -o $outFile -d
        done

        # DNA dictionary.
        ./build/split_index --index-type $iType --hash-type $hType --max-load-factor $maxLF --iter $nIter -i ${inputDir}/dict_dna_danio_rerio.txt -I ${inputDir}/queries_dna_danio_rerio.txt -o $outFile -d
    done
done
