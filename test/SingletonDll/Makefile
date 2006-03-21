_OS = $(shell uname -s)
ifeq ($(_OS), Darwin)
  SHARED_LIB_FILENAME = dylib
  SHARED_LIB_FLAG = -dynamiclib
else
  SHARED_LIB_FILENAME = so
  SHARED_LIB_FLAG = --shared
endif

BIN = client
LIB1 = libfoo.$(SHARED_LIB_FILENAME)
LIB2 = libsingletondll.$(SHARED_LIB_FILENAME)
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
	$(CXX) $(SHARED_LIB_FLAG)  $(CXXFLAGS) $(CPPFLAGS) -o $(LIB1) foo.cpp 

$(LIB2): singletondll.cpp
	$(CXX) $(SHARED_LIB_FLAG)  $(CXXFLAGS) $(CPPFLAGS) -o $(LIB2) singletondll.cpp -L. -lfoo

clean:
	rm -f $(BIN) client.o foo.o singleton.o $(LIB1) $(LIB2)
