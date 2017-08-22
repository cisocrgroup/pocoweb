.PHONY: install
install: pcwd config.ini misc/systemd/pocoweb.service
	$(call ECHO,$@)
	$V install -d $(dir $(BINDIR))
	$V install -d $(DESTDIR)/etc/pocoweb
	$V install -d $(DESTDIR)/usr/lib/systemd/system
	$V install pcwd $(BINDIR)/pcwd
	$V install config.ini $(DESTDIR)/etc/pocoweb/config.ini
	$V install misc/systemd/pocoweb.service $(DESTDIR)/usr/lib/systemd/system
