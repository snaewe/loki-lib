.PHONY: build check install clean
build:
	$(MAKE) -C src

check: build
	$(MAKE) -C test

install:
	$(MAKE) -C src install
	$(MAKE) -C include install

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
