OBJ = main.o
BIN = main-gcc
CXXFLAGS = -Wall -O2
CPPFLAGS = -I../../include -DNDEBUG

$(BIN): $(OBJ)
	$(CXX) -o $(BIN) $(OBJ)

.PHONY: clean
clean:
	rm -f $(OBJ) $(BIN)
