# misc/Makefile

default: ../config.ini

.PHONY: clean install uninstall
clean: 
	$(RM) ../config.ini
install:
uninstall:

../config.ini: default/config.def.ini
	@cp $< $@
