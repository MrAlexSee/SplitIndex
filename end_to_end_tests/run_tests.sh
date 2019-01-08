outFile="res.txt"

printf "1/4 Rebuilding\n"
cd ..
make rebuild

printf "2/4 Copying\n"
cp build/split_index end_to_end_tests/split_index
cd end_to_end_tests

printf "3/4 Running\n"

allTestsPassed=1

# All hash types.
for hType in city farm farsh fnv1 fnv1a murmur3 sdbm spookyv2 superfast xxhash;
do
    # Some selected maximum load factor values.
    for maxLF in 0.5 1.0 1.5 2.25 2.5 3 4 5 6 10
    do
        # All index types for k = 1.
        for iType in k1 k1comp k1comptriple;
        do
            ./split_index --index-type $iType --hash-type $hType --max-load-factor $maxLF -i dict_test.txt -I queries_test.txt --min-word-length 2 > $outFile
            python check_result.py 3

            if [ $? -eq 1 ]
            then
                allTestsPassed=0
            fi
        done

        # k = 2
        ./split_index --index-type k2 --hash-type $hType --max-load-factor $maxLF -i dict_test.txt -I queries_test.txt --min-word-length 3 > $outFile
        python check_result.py 5

        if [ $? -eq 1 ]
        then
            allTestsPassed=0
        fi

        # k = 3
        ./split_index --index-type k3 --hash-type $hType --max-load-factor $maxLF -i dict_test.txt -I queries_test.txt --min-word-length 4 > $outFile
        python check_result.py 2

        if [ $? -eq 1 ]
        then
            allTestsPassed=0
        fi
    done
done

if [ $allTestsPassed -eq 1 ]
then
    printf "\nAll tests PASSED\n"
else
    printf "\nERRORS occurred\n"
fi

printf "4/4 Teardown\n"
rm -f split_index $outFile
