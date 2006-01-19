BIN = main
CXXFLAGS = -Wall -Wold-style-cast -Wundef -pedantic -O2
CPPFLAGS = -I../../include -DNDEBUG
LDFLAGS = -L../../lib
LDLIBS = -lloki

.PHONY: build clean
build: $(BIN)
clean:
	rm -f $(BIN) $(BIN).exe $(BIN).o
