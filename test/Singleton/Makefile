OBJ1 = DeletableSingleton.o
OBJ2 = Dependencies.o
OBJ3 = Phoenix.o
BIN1 = deletable-singleton-gcc
BIN2 = dependencies-gcc
BIN3 = phoenix-gcc
CXXFLAGS = -I../../include -Wall -O2
LIBS = -L../../lib -lloki

.PHONY: all
all: $(BIN1) $(BIN2) $(BIN3)

$(BIN1): $(OBJ1)
	$(CXX) -o $(BIN1) $(OBJ1) $(LIBS)

$(BIN2): $(OBJ2)
	$(CXX) -o $(BIN2) $(OBJ2) $(LIBS)

$(BIN3): $(OBJ3)
	$(CXX) -o $(BIN3) $(OBJ3) $(LIBS)

.PHONY: clean
clean:
	rm -f $(OBJ1) $(BIN1)
	rm -f $(OBJ2) $(BIN2)
	rm -f $(OBJ3) $(BIN3)
