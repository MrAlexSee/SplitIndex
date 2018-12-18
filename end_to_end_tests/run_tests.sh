outFile="res.txt"

echo "1/4 Rebuilding"
cd ..
make rebuild

echo "2/4 Copying"
cp build/split_index end_to_end_tests/split_index
cd end_to_end_tests

echo "3/4 Running"

# All index types for k=1.
for iType in k1 k1comp k1comptriple;
do
    # All hash types.
    for hType in city farm farsh fnv1 fnv1a murmur3 sdbm spookyv2 superfast xxhash;
    do
        # Some selected maximum load factor values.
        for maxLF in 0.5 1.0 1.5 2.25 2.5 3 4 5 6 10
        do
            ./split_index --index-type $iType --hash-type $hType --max-load-factor $maxLF -i dict_test.txt -I queries_test.txt --min-word-length 2 > $outFile
            python check_result.py 3
        done
    done
done

echo "4/4 Teardown"
rm -f split_index $outFile
