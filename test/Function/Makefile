BIN = FunctionTest$(BIN_SUFFIX)
SRC = FunctionTest.cpp
OBJ = $(SRC:.cpp=.o)
override LDLIBS += -lboost_test_exec_monitor

.PHONY: all clean
all: $(BIN)
clean:
	$(RM) $(BIN)
	$(RM) $(OBJ)

$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)
