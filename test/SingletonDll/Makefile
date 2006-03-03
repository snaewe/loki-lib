BIN = client
LIB1 = libfoo.so
LIB2 = libsingletondll.so
#mingw names
#LIB1 = foo.dll
#LIB2 = singletondll.dll
CXXFLAGS = -Wall -Wold-style-cast -Wundef -Wsign-compare -Wconversion -Wpointer-arith -pedantic -O2
CPPFLAGS = -I../../include -DNDEBUG
LDFLAGS = -L../../lib
LDLIBS = -lloki

.PHONY: build clean
build: $(BIN)

$(BIN): $(LIB1) $(LIB2)
	$(CXX) $(CXXFLAGS) -L. $(LDFLAGS) $(CPPFLAGS) -o $(BIN) client.cpp $(LDLIBS) -lfoo -lsingletondll

$(LIB1): foo.cpp
	$(CXX) --shared $(CXXFLAGS) $(CPPFLAGS) -o $(LIB1) foo.cpp 

$(LIB2): singletondll.cpp
	$(CXX) --shared $(CXXFLAGS) $(CPPFLAGS) -o $(LIB2) singletondll.cpp -L. -lfoo

clean:
	rm -f $(BIN) $(LIB1) $(LIB2)
