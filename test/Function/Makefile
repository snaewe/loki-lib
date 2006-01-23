BIN = FunctionTest
CXXFLAGS = -Wall -Wold-style-cast -Wundef -Wsign-compare -Wconversion -Wpointer-arith -pedantic -O2
CPPFLAGS = -I../../include -DNDEBUG
LDFLAGS = -L../../lib
LDLIBS = -lloki -lboost_test_exec_monitor

.PHONY: build clean
build: $(BIN)
clean:
	rm -f $(BIN) $(BIN).exe $(BIN).o
