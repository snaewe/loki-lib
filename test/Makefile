SUBTARGETS = Factory flex_string Longevity OrderedStatic Pimpl RegressionTest \
	SafeFormat ScopeGuard Singleton SmallObj Visitor 
SUBTARGETS_CLEAN = $(foreach SUBTARGET,$(SUBTARGETS),$(SUBTARGET)-clean)

.PHONY: clean all $(SUBTARGETS) $(SUBTARGETS_CLEAN)
all: $(SUBTARGETS)
clean: $(SUBTARGETS_CLEAN)

$(SUBTARGETS):
	$(MAKE) -C $@

$(SUBTARGETS_CLEAN):
	$(MAKE) -C $(@:-clean=) clean
