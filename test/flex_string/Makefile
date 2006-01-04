OBJ = main.o
BIN = main-gcc
CXXFLAGS = -I../../include -Wall -O2

$(BIN): $(OBJ)
	$(CXX) -o $(BIN) $(OBJ)

.PHONY: clean
clean:
	rm -f $(OBJ) $(BIN)
