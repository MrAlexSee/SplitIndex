CC         = clang++
CCFLAGS    = -Wall -pedantic -funsigned-char -msse4.2 -std=c++11
# Try testing both without and with the optimization.
# OPTFLAGS = -DNDEBUG -DNO_ERROR_MSG -O3

BOOST_DIR  = "/home/alex/boost_1_67_0"
INCLUDE    = -I$(BOOST_DIR)

TEST_FILES = catch.hpp repeat.hpp

EXE 	   = main_tests
OBJ        = main_tests.o utils_distance_tests.o utils_string_utils_tests.o

UTILS_LIB  = utils.a

LIB_DIR    = "../build/libs"

LIBS = $(UTILS_LIB)
LIBS := $(addprefix $(LIB_DIR)/,$(LIBS))

all: $(EXE)

$(EXE): $(OBJ) libs
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(OBJ) $(LIBS) -o $@

libs:
	$(MAKE) -C ../src/utils

main_tests.o: main_tests.cpp catch.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c main_tests.cpp

utils_distance_tests.o: utils_distance_tests.cpp ../src/utils/distance.hpp ../src/utils/distance.cpp catch.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c utils_distance_tests.cpp

utils_string_utils_tests.o: utils_string_utils_tests.cpp ../src/utils/string_utils.hpp ../src/utils/string_utils.cpp catch.hpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -c utils_string_utils_tests.cpp

run: all
	./$(EXE)

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ)

rebuild: clean all
