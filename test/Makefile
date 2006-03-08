SUBTARGETS = \
		DeletableSingleton \
		Factory \
		Longevity \
		OrderedStatic \
		Pimpl \
		Register \
		RegressionTest \
		SafeFormat \
		ScopeGuard \
		Singleton \
		SmallObj \
		SmartPtr \
		Visitor \
		flex_string \
		SingletonDll
	
SUBTARGETS_CLEAN = $(foreach SUBTARGET,$(SUBTARGETS),$(SUBTARGET)-clean)

.PHONY: clean all $(SUBTARGETS) $(SUBTARGETS_CLEAN)
all: $(SUBTARGETS)
clean: $(SUBTARGETS_CLEAN)

$(SUBTARGETS):
	$(MAKE) -C $@

$(SUBTARGETS_CLEAN):
	$(MAKE) -C $(@:-clean=) clean
