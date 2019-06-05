## Basic information

This software is called **SplitIndex** (split index).
It can be used for dictionary matching with mismatches (using Hamming distance).
It is based on the Dirichlet principle, which is realized by storing substrings of words from the dictionary.
Authors: Aleksander Cisłak, Szymon Grabowski.

Published as *A Practical Index for Approximate Dictionary Matching with Few Mismatches* (Cisłak and Grabowski), Computing and Informatics, 05/2017, pp. 1088–1106.
Available under the following link: http://www.cai.sk/ojs/index.php/cai/article/view/2017_5_1088.

## Compilation

Add Boost library to the path for compilation by setting `BOOST_DIR` in `makefile.inc`. 
Requires Boost program options module to be compiled for static linking.
Requires support for the C++11 standard and SSE 4.2 instruction set.

Type `make` for optimized compile.
Comment out `OPTFLAGS` in `makefile.inc` in order to disable optimization.

Tested with gcc 64-bit 7.2.0 and Boost 1.67.0 (the latter is not performance-critical, used only for parameter and data parsing and formatting) on Ubuntu 17.10 Linux version 4.13.0-36 64-bit.

## Usage

The default executable path is `build/split_index`.

Basic usage: `./split_index [options] <input dictionary file> <input pattern file>`

Input dictionary file (positional parameter 1 or named parameter `-i` or `--in-dict-file`) should contain the list of words, separated with newline characters.
Input pattern file (positional parameter 2 or named parameter `-I` or `--in-pattern-file`) should contain the list of patterns, separated with newline characters.
Attached as part of this package is a script `test_all.sh` for processing multiple dictionaries.

* End-to-end tests are located in the `end_to_end_tests` folder and they can be run using the `run_tests.sh` script in that folder.
* Unit tests are located in the `unit_tests` folder and they can be run by issuing the `make run` command in that folder (requires support for the C++14 standard).
* The `scripts` directory contains some helpful Python 2 tools.

#### Command-line parameter description

Short name | Long name                | Parameter description
---------- | ------------------------ | ---------------------
`-d`       | `--dump`                 | dump input files and params info with elapsed time to output file (useful for testing)
&nbsp;     | `--dump-all-matches`     | dump the number of matches for each query to standard output, note: this invalidates time measurement
&nbsp;     | `--hash-type`            | hash type used by the split index: city, farm, farsh, fnv1, fnv1a, murmur3, sdbm, spookyv2, superfast, xxhash (default = xxhash)
`-h`       | `--help`                 | display help message
&nbsp;     | `--index-type`           | split index type: k1 (k = 1), k1comp (k = 1 with compression), k1comptriple (k = 1 with 2-,3-,4-gram compression), k2 (k = 2), k3 (k = 3) (default = k1)
`-i`       | `--in-dict-file arg`     | input dictionary file path (positional arg 1)
`-I`       | `--in-pattern-file arg`  | input pattern file path (positional arg 2)
&nbsp;     | `--iter arg`             | number of iterations per pattern lookup (default = 1)
&nbsp;     | `--max-load-factor arg`  | maximum load factor which causes rehashing when crossed (default = 2)
&nbsp;     | `--min-word-length arg`  | minimum word length from input dictionary and queries (shorter words are ignored) (default = 4)
`-o`       | `--out-file arg`         | output file path (default = res.txt)
`-s`       | `--separator arg`        | input data (dictionary and patterns) separator (default = newline)
`-v`       | `--version`              | display version info

#### Data files description

All files listed below are located in the `data` folder.

File name                           | Description
----------------------------------- | -------------------
`dict_dna_danio_rerio.txt`          | words of length 20 extracted from the Danio rerio (zebrafish) genome, 578,964 words
`dict_english_foster.txt`           | English dictionary, 349,900 words
`dict_english_iamerican_insane.txt` | English dictionary, 350,518 words
`queries_dna_danio_rerio.txt`       | queries of length 20 extracted from the Danio rerio (zebrafish) genome, 11,802 words
`queries_english_misspellings.txt`  | a list of common English misspellings, 4,278 words

References:

* Danio rerio genome: https://www.ensembl.org/Danio_rerio/Info/Index
* foster: http://www.math.sjsu.edu/~foster/dictionary.txt
* iamerican-insane: Ubuntu package
* English misspellings: https://en.wikipedia.org/wiki/Wikipedia:Lists_of_common_misspellings/For_machines
