CPPFILES  := $(wildcard src/**/*.cc) $(wildcard src/*.cc)
CC        = g++
GDB       = gdb
LD        = g++
OBJ      := $(patsubst src/%.cc,build/%.o,$(CPPFILES))

MAKE_DIR = @mkdir -p $(@D)

CFLAGS = -std=c++17         \
     -L../lib/build/ -lproofpea \
		 -g                 \
		 -O1                \
		 -I ./include/			\
		 -I ../lib/include

EXECUTABLE = build/main

.PHONY: clean

.DEFAULT_GOAL = $(EXECUTABLE)

all: $(EXECUTABLE) $(CPPFILES)

run: $(EXECUTABLE) $(CPPFILES)
	LD_LIBRARY_PATH=$(realpath ../lib/build) ./$(EXECUTABLE)

debug: $(EXECUTABLE) $(CPPFILES)
	${GDB} $(EXECUTABLE)

build/%.o: src/%.cc include/%.h
	$(MAKE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(EXECUTABLE): $(OBJ)
	$(LD) -o $@ $(OBJ) -L../lib/build/ -lproofpea -lgvc -lxdot -lpathplan -lcgraph -lcdt -g

clean:
	rm -rf $(EXECUTABLE_WIN) $(EXECUTABLE) logs/*

cleaner:
	rm -rf $(EXECUTABLE_WIN) $(EXECUTABLE) $(OBJ) logs/*

valgrind:
	@mkdir -p logs
	valgrind --leak-check=full \
						--show-leak-kinds=all \
						--track-origins=yes \
						--verbose \
						--log-file=logs/valgrind-out.txt \
						$(EXECUTABLE)
