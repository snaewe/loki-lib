OBJ = main.o
BIN = main-gcc
CXXFLAGS = -I../../include -Wall -O2 -DNDEBUG
LIBS = -L../../lib -lloki

$(BIN): $(OBJ)
	$(CXX) -o $(BIN) $(OBJ) $(LIBS)

.PHONY: clean
clean:
	rm -f $(OBJ) $(BIN)
