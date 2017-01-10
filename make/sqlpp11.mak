VENDOR_MAKEFILES += $(VENDOR)/sqlpp11/Makefile
VENDOR_MAKEFILES += $(VENDOR)/sqlpp11-connector-mysql/Makefile

$(VENDOR)/sqlpp11/Makefile: $(MODS)
	mkdir -p $(shell dirname $@)
	cd $(shell dirname $@) && \
		cmake -DHinnantDate_ROOT_DIR=../../modules/date \
			-DCMAKE_INSTALL_PREFIX=.\
			../../modules/sqlpp11
	cd $(shell dirname $@) && $(MAKE) -j$(J) install

$(VENDOR)/sqlpp11-connector-mysql/Makefile: $(MODS)
	mkdir -p $(shell dirname $@)
	cd $(shell dirname $@) && \
		cmake -DCMAKE_INSTALL_PREFIX=.\
			../../modules/sqlpp11-connector-mysql
	cd $(shell dirname $@) && $(MAKE) -j$(J) install

ALL += $(VENDOR_MAKEFILES)
CXXFLAGS += -I$(VENDOR)/sqlpp11/include
CXXFLAGS += -Imodules/sqlpp11-connector-mysql/include
CXXFLAGS += -Imodules/date
LDFLAGS += -L$(VENDOR)/sqlpp11-connector-mysql/lib
LDFLAGS += -lsqlpp-mysql -lmysqlclient
