ROOT_DIR = ../..
LIB_NAME = hash_function.a

include $(ROOT_DIR)/Makefile.inc

CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(addprefix $(ROOT_DIR)/obj/,$(notdir $(CPP_FILES:.cpp=.o)))

all: $(ROOT_DIR)/lib/$(LIB_NAME)

$(ROOT_DIR)/lib/$(LIB_NAME): $(OBJ_FILES)
	ar rs $@ $(OBJ_FILES)

-include $(OBJ_FILES:.o=.d)

$(ROOT_DIR)/obj/%.o: %.cpp 
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -MMD -c $< -o $@
