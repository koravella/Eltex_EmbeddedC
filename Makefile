SUBDIRS := $(wildcard */.)

.PHONY: all $(SUBDIRS) clean

all clean: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
