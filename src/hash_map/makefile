ROOT_DIR = ../..

include $(ROOT_DIR)/makefile.inc

CPP_FILES = $(wildcard *.cpp)
OBJ_FILES = $(addprefix $(ROOT_DIR)/$(OBJ_DIR)/,$(notdir $(CPP_FILES:.cpp=.o)))

LIB = $(ROOT_DIR)/$(LIB_DIR)/$(HASH_MAP_LIB)

all: $(LIB)

$(LIB): $(OBJ_FILES)
	ar rs $@ $(OBJ_FILES)

-include $(OBJ_FILES:.o=.d)

$(ROOT_DIR)/$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -MMD -c $< -o $@
