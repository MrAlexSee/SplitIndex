include makefile.inc

BUILD_DIR = build
LIB_DIR   = $(BUILD_DIR)/libs
OBJ_DIR   = $(BUILD_DIR)/obj

all: dirs $(BUILD_DIR)/$(EXE)

MAIN_LIB   		  = main.a
HASH_FUNCTION_LIB = hash_function.a
HASH_MAP_LIB      = hash_map.a
INDEX_LIB 		  = index.a
UTILS_LIB  		  = utils.a

# The order here is based on library dependencies (left depends on right).
LIBS = $(MAIN_LIB) $(INDEX_LIB) $(HASH_MAP_LIB) $(HASH_FUNCTION_LIB) $(UTILS_LIB)
LIBS := $(addprefix lib/,$(LIBS))

build/$(EXE): libs
	$(CC) $(LDFLAGS) $(OPTFLAGS) $(LIBS) $(AUXFLAGS) -o $@

libs:
	$(MAKE) -C src/hash_function
	$(MAKE) -C src/hash_map
	$(MAKE) -C src/index
	$(MAKE) -C src/main
	$(MAKE) -C src/utils

.PHONY: dirs
dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(LIB_DIR)
	mkdir -p $(OBJ_DIR)

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/$(EXE)
	rm -f $(LIB_DIR)/*.a
	rm -f $(OBJ_DIR)/*.d $(OBJ_DIR)/*.o

rebuild: clean all
