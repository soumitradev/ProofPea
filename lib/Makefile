CPPFILES  := $(wildcard src/**/*.cc) $(wildcard src/*.cc)
CC        = g++
GDB       = gdb
LD        = g++
OBJ      := $(patsubst src/%.cc,build/%.o,$(CPPFILES))

MAKE_DIR = @mkdir -p $(@D)

CFLAGS = -std=c++17         \
		 -fPIC							\
		 -O1                \
		 -I ./include/

LIBRARY_SHARED = build/libproofpea.so
LIBRARY_STATIC = build/libproofpea.a

.PHONY: clean

.DEFAULT_GOAL = $(LIBRARY_SHARED) $(LIBRARY_STATIC)

all: $(LIBRARY_SHARED) $(LIBRARY_STATIC) $(CPPFILES)

build/%.o: src/%.cc include/%.h
	$(MAKE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(LIBRARY_SHARED): $(OBJ)
	$(LD) -shared -lgvc -lxdot -lpathplan -lcgraph -lcdt -o $@ $(OBJ)
	
$(LIBRARY_STATIC): $(OBJ)
	ar rcs $@ $(OBJ)

clean:
	rm -rf $(LIBRARY_SHARED) $(LIBRARY_STATIC) logs/*

cleaner:
	rm -rf $(LIBRARY_SHARED) $(LIBRARY_STATIC) $(OBJ) logs/*

