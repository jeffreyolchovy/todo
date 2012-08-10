TARGETS=all install uninstall clean distclean test

$(TARGETS):
	@cd src && $(MAKE) $@

PHONY: install
