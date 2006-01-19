BIN1 = DeletableSingleton
BIN2 = Dependencies
BIN3 = Phoenix
CXXFLAGS = -Wall -Wold-style-cast -Wundef -pedantic -O2
CPPFLAGS = -I../../include -DNDEBUG
LDFLAGS = -L../../lib
LDLIBS = -lloki

.PHONY: build clean
build: $(BIN1) $(BIN2) $(BIN3)
clean:
	rm -f $(BIN1) $(BIN2) $(BIN3)  $(BIN1).exe $(BIN1).o $(BIN2).exe $(BIN2).o $(BIN3).exe $(BIN3).o
