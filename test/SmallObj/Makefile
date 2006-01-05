OBJ1 = SmallObjBench.o
OBJ2 = SmallSingleton.o
BIN1 = small-obj-bench-gcc
BIN2 = small-singleton-gcc
CXXFLAGS = -I../../include -Wall -fexpensive-optimizations -O3 -DNDEBUG
LIBS = -L../../lib -lloki

.PHONY: all
all: $(BIN1) $(BIN2)

$(BIN1): $(OBJ1)
	$(CXX) -o $(BIN1) $(OBJ1) $(LIBS)

$(BIN2): $(OBJ2)
	$(CXX) -o $(BIN2) $(OBJ2) $(LIBS)

.PHONY: clean
clean:
	rm -f $(OBJ1) $(BIN1)
	rm -f $(OBJ2) $(BIN2)
