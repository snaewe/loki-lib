
MAKEFILE = Makefile

SUBTARGETS	= \
		sub-src \
		sub-test \

all: $(SUBTARGETS)

sub-src: FORCE
	cd src && \
	$(MAKE) -f $(MAKEFILE)
	@cd ..

sub-test: FORCE
	cd test && \
	$(MAKE) -f $(MAKEFILE)
	@cd ..

	
	
clean: 
	cd src && \
	$(MAKE) -f $(MAKEFILE) clean
	@cd ..
	cd test && \
	$(MAKE) -f $(MAKEFILE) clean
	@cd ..




FORCE:

