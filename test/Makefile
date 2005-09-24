
MAKEFILE = Makefile.mingw

SUBTARGETS	=  \
		sub-flex_string \
		sub-SafeFormat \
		sub-SmallObj

all: $(SUBTARGETS)

sub-flex_string: FORCE
	cd flex_string && \
	$(MAKE) -f $(MAKEFILE)
	@cd ..

sub-SafeFormat: FORCE
	cd SafeFormat && \
	$(MAKE) -f $(MAKEFILE)
	@cd ..

sub-SmallObj: FORCE
	cd SmallObj && \
	$(MAKE) -f $(MAKEFILE)
	@cd ..
	
	
	
clean: 
	cd flex_string && \
	$(MAKE)  -f $(MAKEFILE) clean
	@cd ..
	cd SafeFormat && \
	$(MAKE)  -f $(MAKEFILE) clean
	@cd ..
	cd SmallObj && \
	$(MAKE)  -f $(MAKEFILE) clean
	@cd ..




FORCE:

