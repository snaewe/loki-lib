.PHONY: build-static build-shared check install install-includes install-shared clean-tmp clean
build-static:
	$(MAKE) -C src build-static

build-shared:
	$(MAKE) -C src build-shared

check: build-static
	$(MAKE) -C test

install: install-includes
	$(MAKE) -C src install-static

install-includes:
	$(MAKE) -C include install

install-shared: install-includes
	$(MAKE) -C src install-shared

clean-tmp:
	$(MAKE) -C src clean-tmp

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
