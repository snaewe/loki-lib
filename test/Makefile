SUBTARGETS = Factory flex_string Longevity OrderedStatic RegressionTest \
	SafeFormat ScopeGuard Singleton SmallObj Visitor Pimpl

.PHONY: clean all $(SUBTARGETS)
all: $(SUBTARGETS)

$(SUBTARGETS):
	$(MAKE) -C $@

clean:
	for subtarget in $(SUBTARGETS); do $(MAKE) -C $$subtarget clean ; done
