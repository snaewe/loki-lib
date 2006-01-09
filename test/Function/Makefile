BIN = FunctionTest
CXXFLAGS = -Wall -O2
CPPFLAGS = -I../../include -DNDEBUG
LDFLAGS = -L../../lib
LDLIBS = -lloki -lboost_test_exec_monitor

.PHONY: build clean
build: $(BIN)
clean:
	rm -f $(BIN)
