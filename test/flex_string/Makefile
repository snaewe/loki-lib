BIN = main$(BIN_SUFFIX)
SRC = main.cpp
OBJ = $(SRC:.cpp=.o)
override LDFLAGS =
override LDLIBS =

.PHONY: all clean
all: $(BIN)
clean:
	$(RM) $(BIN)
	$(RM) $(OBJ)

$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)
