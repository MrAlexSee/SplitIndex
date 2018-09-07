CC        = g++
CCFLAGS   = -Wall -pedantic -funsigned-char -msse4.2 -std=c++11
OPTFLAGS  = -DNDEBUG -DNO_ERROR_MSG -O3

BOOST_DIR = "/home/alex/boost_1_67_0"

INCLUDE   = -I$(BOOST_DIR)
LDFLAGS   = -L$(BOOST_DIR) -static
LDLIBS    = -lboost_program_options

EXE       = split_index

BUILD_DIR = build
LIB_DIR   = $(BUILD_DIR)/libs
OBJ_DIR   = $(BUILD_DIR)/obj

MAIN_LIB   		  = main.a
HASH_FUNCTION_LIB = hash_function.a
HASH_MAP_LIB      = hash_map.a
INDEX_LIB 		  = index.a
UTILS_LIB  		  = utils.a
