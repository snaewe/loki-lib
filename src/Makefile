SRC = $(wildcard *.cpp)
STATIC_OBJ = $(SRC:.cpp=.o)
SHARED_OBJ = $(SRC:.cpp=.lo)
_OS = $(shell uname -s)

LOKI_VERSION = 0.1.4

STATIC_LIB = ../lib/libloki.a
ifeq ($(_OS), Darwin)
  SHARED_LIB_INSTALL = 
  SHARED_LIB_FLAG = -dynamiclib
  SHARED_LIB = ../lib/libloki.$(LOKI_VERSION).dylib
else
  SHARED_LIB_FLAG = --shared
  SHARED_LIB_INSTALL = -Wl,-soname=libloki.$(SHARED_LIB_EXTENSION).$(LOKI_VERSION)
  SHARED_LIB = ../lib/libloki.so.$(LOKI_VERSION)
endif


CXXFLAGS = -Wall -Wold-style-cast -Wundef -Wsign-compare -Wconversion -Wpointer-arith -pedantic -O2
CPPFLAGS = -I../include -DNDEBUG

$(STATIC_LIB): $(STATIC_OBJ)
	ar rs $(STATIC_LIB) $(STATIC_OBJ)

$(SHARED_LIB): $(SHARED_OBJ)
	g++ $(SHARED_LIB_FLAG) $(SHARED_LIB_INSTALL) -o $(SHARED_LIB) $(SHARED_OBJ)

%.lo : %.cpp
	$(CXX) -c $(CXXFLAGS) -fPIC $(CPPFLAGS) -o $@ $<

.PHONY: build-static build-shared install-static install-shared clean-tmp clean
build-static: $(STATIC_LIB)
build-shared: $(SHARED_LIB)

install-static: $(STATIC_LIB)
	mkdir -p $(DESTDIR)/usr/lib
	install -m 644 $(STATIC_LIB) $(DESTDIR)/usr/lib

install-shared: $(SHARED_LIB)
	mkdir -p $(DESTDIR)/usr/lib
	install -m 644 -s $(SHARED_LIB) $(DESTDIR)/usr/lib
	cd $(DESTDIR)/usr/lib/; ln -s libloki.so.$(LOKI_VERSION) libloki.so

clean-tmp:
	rm -f $(STATIC_OBJ)
	rm -f $(SHARED_OBJ)

clean: clean-tmp
	rm -f $(STATIC_LIB)
	rm -f $(SHARED_LIB)
