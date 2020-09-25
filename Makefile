SUBDIRS := ./prac_1

$(SUBDIRS):
	$(MAKE) -C $@ -s

.PHONY: $(SUBDIRS)