ROOT_DIR = ../..

include $(ROOT_DIR)/makefile.inc

CPP_FILES = $(wildcard *.cpp)
OBJ_FILES = $(addprefix $(ROOT_DIR)/$(OBJ_DIR)/,$(notdir $(CPP_FILES:.cpp=.o)))

LIB = $(ROOT_DIR)/$(LIB_DIR)/$(INDEX_LIB)

all: $(LIB)

$(LIB): $(OBJ_FILES) split_index_k.hpp
	ar rs $@ $(OBJ_FILES)

-include $(OBJ_FILES:.o=.d)

$(ROOT_DIR)/$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -MMD -c $< -o $@
