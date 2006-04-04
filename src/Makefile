SRC = $(wildcard *.cpp)
STATIC_OBJ = $(SRC:.cpp=.o)
SHARED_OBJ = $(SRC:.cpp=.lo)

VERSION = 0.1.4
DESTDIR = /usr

CXXFLAGS = -Wall -Wold-style-cast -Wundef -Wsign-compare -Wconversion -Wpointer-arith -pedantic -O2
CPPFLAGS = -I../include -DNDEBUG

STATIC_LIB = ../lib/libloki.a

ifeq ($(shell uname -s), Darwin)
  SO_EXT = dylib
  INSTALL_FLAGS =
  INSTALL_TARGET = libloki.$(VERSION).$(SO_EXT)
  LDFLAGS = -dynamiclib -single_module -install_name $(INSTALL_TARGET)
else
  SO_EXT = so
  INSTALL_FLAGS = -s
  INSTALL_TARGET = libloki.$(SO_EXT).$(VERSION)
  LDFLAGS = --shared -Wl,-soname=$(INSTALL_TARGET)
endif

SHARED_LIB = ../lib/$(INSTALL_TARGET)

$(STATIC_LIB): $(STATIC_OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(SHARED_LIB): $(SHARED_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

%.lo : %.cpp
	$(CXX) -c $(CXXFLAGS) -fPIC $(CPPFLAGS) -o $@ $<

.PHONY: build-static build-shared install-static install-shared clean-tmp clean
build-static: $(STATIC_LIB)
build-shared: $(SHARED_LIB)

install-static: $(STATIC_LIB)
	mkdir -p $(DESTDIR)/lib
	install -m 644 $< $(DESTDIR)/lib

install-shared: $(SHARED_LIB)
	mkdir -p $(DESTDIR)/lib
	install -m 644 $(INSTALL_FLAGS) $< $(DESTDIR)/lib
	cd $(DESTDIR)/lib/ && $(RM) libloki.$(SO_EXT) && ln -s $(INSTALL_TARGET) libloki.$(SO_EXT)

clean-tmp:
	$(RM) $(STATIC_OBJ)
	$(RM) $(SHARED_OBJ)

clean: clean-tmp
	$(RM) $(STATIC_LIB)
	$(RM) $(SHARED_LIB)
