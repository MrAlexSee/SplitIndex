### Basic information

This software is called **SplitIndex**, since it is based on the Dirichlet principle (storing parts of words). It can be used for dictionary matching with mismatches.

Published as *A Practical Index for Approximate Dictionary Matching with Few Mismatches* (Cisłak and Grabowski), Computing and Informatics, 05/2017.
Also available as a preprint at the following link: http://arxiv.org/abs/1501.04948.

### Compilation

1. Select the compiler (`CC`) in `Makefile.inc` (tested on clang version 3.4-1ubuntu3).
1. Set the path to the Boost library (`INCLUDE`) in `Makefile.inc` (tested on Boost 1.57).
1. Select the debug or release version (`OPTFLAGS`) in `Makefile.inc`.
1. Tweak the parameters in `src/main/params.hpp`.
1. Type `make`.

### Usage

1. The executable path is `build/main`.
1. Usage: `build/main [dict] [opt: queries]`
   The first parameter is a text file with the input (the database of keywords). Any whitespace is a default separator for words. The second parameter is a text file with queries. If not supplied, queries are randomly generated from the input.
1. Optionally, set `INPUT` and `QUERIES` in `Makefile.inc` and type `make run`.
1. The executable will print the sizes (input after filtering and index size) and the elapsed time.

### Testing correctness

1. Set the index you want to check in `src/main/params.hpp`.
1. Set the same `k` in `testing/naive_hamming.py` (`hamK`).
1. Set the same `minWordLength` (minimum word length) in `testing/naive_hamming.py` and `src/main/params.hpp`.
1. Type `make rebuild` in the main folder.
1. Set the paths in `testing/run_and_compare.sh`.
1. Go to testing and type `./run_and_compare.sh"`.
