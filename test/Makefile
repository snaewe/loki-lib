
MAKEFILE = Makefile

SUBTARGETS	= \
		sub-Factory \
		sub-flex_string \
		sub-Longevity \
		sub-OrderedStatic \
		sub-RegressionTest \
		sub-SafeFormat \
		sub-SmallObj 

all: $(SUBTARGETS)

sub-Factory: FORCE
	cd Factory && \
	$(MAKE) -f $(MAKEFILE)
	@cd ..
	
sub-flex_string: FORCE
	cd flex_string && \
	$(MAKE) -f $(MAKEFILE)
	@cd ..

sub-RegressionTest: FORCE
	cd RegressionTest && \
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
	
sub-OrderedStatic: FORCE
	cd OrderedStatic && \
	$(MAKE) -f $(MAKEFILE)
	@cd ..
	
sub-Longevity: FORCE
	cd Longevity && \
	$(MAKE) -f $(MAKEFILE) && \
	$(MAKE) -f $(MAKEFILE)2
	@cd ..
	
clean: 
	cd Factory && \
	$(MAKE) -f $(MAKEFILE) clean
	@cd ..
	cd flex_string && \
	$(MAKE) -f $(MAKEFILE) clean
	@cd ..
	cd Longevity && \
	$(MAKE) -f $(MAKEFILE) clean
	@cd ..
	cd OrderedStatic && \
	$(MAKE) -f $(MAKEFILE) clean
	@cd ..
	cd RegressionTest && \
	$(MAKE) -f $(MAKEFILE) clean
	@cd ..
	cd SafeFormat && \
	$(MAKE) -f $(MAKEFILE) clean
	@cd ..
	cd SmallObj && \
	$(MAKE) -f $(MAKEFILE) clean
	@cd ..



FORCE:

