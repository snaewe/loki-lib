BIN1 = SmallObjBench
BIN2 = SmallObjSingleton
CXXFLAGS = -Wall -fexpensive-optimizations -O3
CPPFLAGS = -I../../include -DNDEBUG
LDFLAGS = -L../../lib
LDLIBS = -lloki

.PHONY: build clean
build: $(BIN1) $(BIN2)
clean:
	rm -f $(BIN1) $(BIN2)
