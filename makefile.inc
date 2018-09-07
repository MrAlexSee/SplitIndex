CC        = g++
CCFLAGS   = -Wall -pedantic -funsigned-char -msse4.2 -std=c++14
OPTFLAGS  = -O3 -m32 -DNDEBUG -DNO_ERROR_MSG

BOOST_DIR = "/home/alex/boost_1_67_0"

INCLUDE   = -I$(BOOST_DIR)
LDFLAGS   = -L$(BOOST_DIR) -static
LDLIBS    = -lboost_program_options

EXE       = main
