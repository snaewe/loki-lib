BIN1 = SmallObjBench
BIN2 = SmallObjSingleton
CXXFLAGS = -Wall -Wold-style-cast -Wundef -pedantic -fexpensive-optimizations -O3
CPPFLAGS = -I../../include -DNDEBUG
LDFLAGS = -L../../lib
LDLIBS = -lloki

.PHONY: build clean
build: $(BIN1) $(BIN2)
clean:
	rm -f $(BIN1) $(BIN2)  $(BIN1).exe $(BIN1).o  $(BIN2).exe $(BIN2).o
