INSTALL_CONFIG = $(DESTDIR)/etc/pocoweb/config.ini
INSTALL_SERVICE = $(DESTDIR)/usr/lib/systemd/system/pocoweb.service
INSTALL_LOGIN = $(DESTDIR)/etc/pocoweb/plugins/simple-login.so
INSTALL_EXE = $(BINDIR)/pcwd
INSTALL_LIB = $(LIBDIR)/libpcw.a

.PHONY: install
install: $(INSTALL_EXE) $(INSTALL_LIB) $(INSTALL_SERVICE) $(INSTALL_CONFIG) $(INSTALL_LOGIN)
.PHONY: uninstall
uninstall:
	$V $(RM) $(INSTALL_CONFIG) $(INSTALL_SERVICE) $(INSTALL_LOGIN) $(INSTALL_EXE) $(INSTALL_LIB)

$(INSTALL_EXE): pcwd
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install $< $@

$(INSTALL_LIB): lib/libpcw.a
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install $< $@

$(INSTALL_LOGIN): lib/simple-login.so
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install $< $@

$(INSTALL_CONFIG): misc/default/config.def.ini
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 600 $< $@

$(INSTALL_SERVICE): misc/systemd/pocoweb.service
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install $< $@

misc/systemd/pocoweb.service: misc/systemd/pocoweb.service.def
	$(call ECHO,$@)
	$V sed  -e 's#$${PCW_LOG_PIDFILE}#${PCW_LOG_PIDFILE}#' 		\
		-e 's#$${PCW_POCOWEB_EXE}#${INSTALL_EXE}#' 		\
		-e 's#$${PCW_POCOWEB_CONFIG}#${INSTALL_CONFIG}#' 	\
		$< > $@
