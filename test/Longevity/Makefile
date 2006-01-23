BIN1 = main
BIN2 = main2
CXXFLAGS = -Wall -Wold-style-cast -Wundef -Wsign-compare -Wconversion -Wpointer-arith -pedantic -fexpensive-optimizations -O3
CPPFLAGS = -I../../include -DNDEBUG
LDFLAGS = -L../../lib
LDLIBS = -lloki

.PHONY: build clean
build: $(BIN1) $(BIN2)
clean:
	rm -f $(BIN1) $(BIN2) $(BIN1).exe $(BIN1).o $(BIN2).exe $(BIN2).o
