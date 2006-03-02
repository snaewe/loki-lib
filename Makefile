.PHONY: build-static build-shared check install clean-tmp clean
build-static:
	$(MAKE) -C src build-static

build-shared:
	$(MAKE) -C src build-shared

check: build-static
	$(MAKE) -C test

install:
	$(MAKE) -C src install-static
	$(MAKE) -C include install

install-shared:
	$(MAKE) -C src install-shared

clean-tmp:
	$(MAKE) -C src clean-tmp

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
