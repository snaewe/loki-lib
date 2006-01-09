BIN1 = DeletableSingleton
BIN2 = Dependencies
BIN3 = Phoenix
CXXFLAGS = -Wall -O2
CPPFLAGS = -I../../include -DNDEBUG
LDFLAGS = -L../../lib
LDLIBS = -lloki

.PHONY: build clean
build: $(BIN1) $(BIN2) $(BIN3)
clean:
	rm -f $(BIN1) $(BIN2) $(BIN3)
