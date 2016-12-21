sqlpp11: $(VENDOR)/sqlpp11/Makefile

$(VENDOR)/sqlpp11/Makefile: $(MODS)
	mkdir -p $(shell dirname $@)
	cd $(shell dirname $@) && \
		cmake -DHinnantDate_ROOT_DIR=../../modules/date \
			-DCMAKE_INSTALL_PREFIX=.\
			../../modules/sqlpp11
	cd $(shell dirname $@) && $(MAKE) -j$(J) install

.PHONY: sqlpp11
ALL += sqlpp11
CXXFLAGS += -I$(VENDOR)/sqlpp11/include
CXXFLAGS += -Imodules/sqlpp11-connector-stl/include
CXXFLAGS += -Imodules/date
