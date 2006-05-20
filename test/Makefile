export CPPFLAGS = -I../../include -DNDEBUG
export LDFLAGS = -L../../lib
export LDLIBS = -lloki

ifeq ($(OS),Windows)
export BIN_SUFFIX = .exe
else
export BIN_SUFFIX =
endif

SUBTARGETS_ORIG := $(patsubst %/,%,$(dir $(wildcard */Makefile)))
SUBTARGETS := $(filter-out $(SUBTARGETS_FILTER_OUT),$(SUBTARGETS_ORIG))
	
SUBTARGETS_CLEAN := $(addsuffix -clean,$(SUBTARGETS))

.PHONY: all clean $(SUBTARGETS) $(SUBTARGETS_CLEAN)
all: $(SUBTARGETS)
clean: $(SUBTARGETS_CLEAN)

$(SUBTARGETS):
	$(MAKE) -C $@

$(SUBTARGETS_CLEAN):
	$(MAKE) -C $(@:-clean=) clean
