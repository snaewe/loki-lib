export VERSION = 0.1.5
export OS ?= $(shell uname -s)

export CXXWARNFLAGS = -Wall -Wold-style-cast -Wundef -Wsign-compare -Wconversion -Wpointer-arith -pedantic
export CXXFLAGS = $(CXXWARNFLAGS) -g -O2

.PHONY: all build-static build-shared check clean distclean
all: build-static build-shared check
build-static:
	$(MAKE) -C src build-static

build-shared:
	$(MAKE) -C src build-shared

check: build-shared
	$(MAKE) -C test

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean

distclean: clean
	$(MAKE) -C src distclean

ifneq ($(OS),Windows)
export prefix ?= /usr

.PHONY: install install-static
install:
	$(MAKE) -C src install-static
	$(MAKE) -C src install-shared
	$(MAKE) -C include install

install-static:
	$(MAKE) -C src install-static
	$(MAKE) -C include install
endif
