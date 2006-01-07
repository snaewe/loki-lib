SUBTARGETS	= Factory flex_string Longevity OrderedStatic RegressionTest \
	SafeFormat ScopeGuard Singleton SmallObj Visitor

.PHONY: clean all $(SUBTARGETS)
all: $(SUBTARGETS)

$(SUBTARGETS):
	$(MAKE) -C $@

clean: 
	$(MAKE) -C Factory clean
	$(MAKE) -C flex_string clean
	$(MAKE) -C Longevity clean
	$(MAKE) -C OrderedStatic clean
	$(MAKE) -C RegressionTest clean
	$(MAKE) -C SafeFormat clean
	$(MAKE) -C ScopeGuard clean
	$(MAKE) -C Singleton clean
	$(MAKE) -C SmallObj clean
	$(MAKE) -C Visitor clean
