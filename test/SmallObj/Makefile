include ../Makefile.common

BIN1 := SmallObjBench$(BIN_SUFFIX)
SRC1 := SmallObjBench.cpp
OBJ1 := $(SRC1:.cpp=.o)
BIN2 := SmallObjSingleton$(BIN_SUFFIX)
SRC2 := SmallObjSingleton.cpp
OBJ2 := $(SRC2:.cpp=.o)
BIN3 := DefaultAlloc$(BIN_SUFFIX)
SRC3 := DefaultAlloc.cpp
OBJ3 := $(SRC1:.cpp=.o)
CXXFLAGS := $(CXXWARNFLAGS) -g -fexpensive-optimizations -O3

.PHONY: all clean
all: $(BIN1) $(BIN2) $(BIN3)
clean: cleandeps
	$(RM) $(BIN1)
	$(RM) $(OBJ1)
	$(RM) $(BIN2)
	$(RM) $(OBJ2)
	$(RM) $(BIN3)
	$(RM) $(OBJ3)

$(BIN1): $(OBJ1)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BIN2): $(OBJ2)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BIN3): $(OBJ3)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

include ../../Makefile.deps
