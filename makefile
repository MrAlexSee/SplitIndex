include makefile.inc

all: dirs $(BUILD_DIR)/$(EXE)

# The order here is based on library dependencies (left depends on right).
LIBS = $(MAIN_LIB) $(INDEX_LIB) $(HASH_MAP_LIB) $(HASH_FUNCTION_LIB) $(UTILS_LIB)
LIBS := $(addprefix $(LIB_DIR)/,$(LIBS))

build/$(EXE): libs
	$(CC) $(LDFLAGS) $(OPTFLAGS) $(LIBS) $(AUXFLAGS) -o $@

libs:
	$(MAKE) -C src/hash_function
	# $(MAKE) -C src/hash_map
	# $(MAKE) -C src/index
	# $(MAKE) -C src/main
	# $(MAKE) -C src/utils

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
