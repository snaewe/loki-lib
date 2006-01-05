OBJ1 = main.o
OBJ2 = main2.o
BIN1 = main-gcc
BIN2 = main2-gcc
CXXFLAGS = -I../../include -Wall -O2 -DNDEBUG
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
