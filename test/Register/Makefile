BIN = main
LIB1 = libfoo.a
CXXFLAGS = -Wall -Wold-style-cast -Wundef -Wsign-compare -Wconversion -Wpointer-arith -pedantic -O2 -DLOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT
CPPFLAGS = -I../../include -DNDEBUG
LDFLAGS = -L../../lib
LDLIBS = -lloki

.PHONY: build clean
build: $(BIN)

$(BIN): $(LIB1) 
	$(CXX) $(CXXFLAGS) -L. $(LDFLAGS) $(CPPFLAGS) -o $(BIN) main.cpp $(LDLIBS) -lfoo

$(LIB1): foo.o
	ar rs $(LIB1) foo.o

%.a : %.cpp
	$(CXX) -c $(CXXFLAGS) -fPIC $(CPPFLAGS) -o $@ $<
	
clean:
	rm -f $(BIN) $(LIB1)
