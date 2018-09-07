CPP_FILES = $(wildcard *.cpp)
OBJ_FILES = $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_FILES:.cpp=.o)))

all: $(LIB_DIR)/$(UTILS_LIB)

$(LIB_DIR)/$(UTILS_LIB): $(OBJ_FILES)
	ar rs $@ $(OBJ_FILES)

-include $(OBJ_FILES:.o=.d)

$(OBJ_DIR)/%.o: %.cpp 
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(INCLUDE) -MMD -c $< -o $@
