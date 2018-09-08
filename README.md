## Basic information

This software is called **SplitIndex**.
It can be used for dictionary matching with mismatches (Hamming distance).
It is based on the Dirichlet principle, which is realized by storing substrings of words from the dictionary.
Authors: Aleksander Cisłak, Szymon Grabowski.

Published as *A Practical Index for Approximate Dictionary Matching with Few Mismatches* (Cisłak and Grabowski), Computing and Informatics, 05/2017.
Also available as a preprint at the following link: http://arxiv.org/abs/1501.04948.

## Compilation

Add Boost library to the path for compilation by setting `BOOST_DIR` in `makefile.inc`. 
Requires Boost program options module to be compiled for static linking.
Requires support for the C++11 standard.

Type `make` for optimized compile.
Comment out `OPTFLAGS` in `makefile.inc` in order to disable optimization.

Tested with gcc 64-bit 7.2.0 and Boost 1.67.0 (the latter is not performance-critical, used only for parameter and data parsing and formatting) on Ubuntu 17.10 Linux version 4.13.0-36 64-bit.

## Usage

The default executable path is `build/split_index`.

Basic usage: `./split_index [options] <input dictionary file> <input pattern file>`

Input dictionary file (positional parameter 1 or named parameter `-i` or `--in-dict-file`) should contain the list of words, separated with newline characters.
Input pattern file (positional parameter 2 or named parameter `-I` or `--in-pattern-file`) should contain the list of patterns, separated with newline characters.
Attached as part of this package is a script `test_all.sh` for processing multiple dictionaries.

#### Command-line parameter description

Short name | Long name                | Parameter description
---------- | ------------------------ | ---------------------
