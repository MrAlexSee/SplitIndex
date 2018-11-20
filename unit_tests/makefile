CC         = clang++
CCFLAGS    = -Wall -pedantic -funsigned-char -msse4.2 -std=c++14
# Try testing both without and with the optimization.
# OPTFLAGS = -DNDEBUG -DNO_ERROR_MSG -O3

BOOST_DIR  = "/home/alex/boost_1_67_0"
INCLUDE    = -I$(BOOST_DIR)

TEST_FILES = catch.hpp repeat.hpp

EXE 	   = main_tests
OBJ        = main_tests.o hash_map_aligned_tests.o split_index_1_tests.o split_index_1_searching_tests.o split_index_1_comp_tests.o utils_distance_tests.o utils_file_io_tests.o utils_string_utils_tests.o

HASH_FUNCTION_LIB  = hash_function.a
HASH_MAP_LIB       = hash_map.a
INDEX_DIR 		   = index.a
UTILS_LIB  	       = utils.a

LIB_DIR = "../build/libs"

LIBS = $(HASH_MAP_LIB) $(HASH_FUNCTION_LIB) $(INDEX_DIR) $(UTILS_LIB)
LIBS := $(addprefix $(LIB_DIR)/,$(LIBS))

all: $(EXE)

$(EXE): $(OBJ) libs
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(OBJ) $(LIBS) -o $@

libs:
	$(MAKE) -C ../src/hash_function
	$(MAKE) -C ../src/hash_map
	$(MAKE) -C ../src/index
	$(MAKE) -C ../src/utils

main_tests.o: main_tests.cpp catch.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c main_tests.cpp

hash_map_aligned_tests.o: hash_map_aligned_tests.cpp ../src/hash_map/hash_map.* ../src/hash_map/hash_map_aligned.* hash_map_aligned_whitebox.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c hash_map_aligned_tests.cpp

split_index_1_tests.o: split_index_1_tests.cpp ../src/index/split_index.* ../src/index/split_index_1.* split_index_1_whitebox.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c split_index_1_tests.cpp

split_index_1_searching_tests.o: split_index_1_searching_tests.cpp ../src/index/split_index.* ../src/index/split_index_1.* $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c split_index_1_searching_tests.cpp

split_index_1_comp_tests.o: split_index_1_comp_tests.cpp ../src/index/split_index.* ../src/index/split_index_1.* split_index_1_comp_whitebox.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c split_index_1_comp_tests.cpp

utils_distance_tests.o: utils_distance_tests.cpp ../src/utils/distance.hpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c utils_distance_tests.cpp

utils_file_io_tests.o: utils_file_io_tests.cpp ../src/utils/file_io.hpp ../src/utils/file_io.cpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c utils_file_io_tests.cpp

utils_string_utils_tests.o: utils_string_utils_tests.cpp ../src/utils/string_utils.hpp ../src/utils/string_utils.cpp $(TEST_FILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c utils_string_utils_tests.cpp

run: all
	./$(EXE)

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ)

rebuild: clean all
