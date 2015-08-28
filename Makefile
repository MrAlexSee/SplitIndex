include Makefile.inc

# The order here is based on library dependencies (left depends on right).
LIBS  = main.a index.a hash_map.a hash_function.a utils.a
LIBS := $(addprefix lib/,$(LIBS))

all: build/$(EXE)
run: all
	 clear
	 ./build/$(EXE) $(INPUT) $(QUERIES)

build/$(EXE): libs
	$(CC) $(LDFLAGS) $(OPTFLAGS) $(LIBS) $(AUXFLAGS) -o $@

libs:
	$(MAKE) -C src/hash_function
	$(MAKE) -C src/hash_map
	$(MAKE) -C src/index
	$(MAKE) -C src/main
	$(MAKE) -C src/utils

.PHONY: clean

clean:
	rm -f build/main
	rm -f lib/*.a
	rm -f obj/*.d obj/*.o

rebuild: clean all
