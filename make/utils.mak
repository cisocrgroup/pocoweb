# make/utils.mak

MAINS += pcwc
pcwc: utils/src/pcwc.o $(LIBS)
	$(call ECHO,$@)
	$V $(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) -lcurl
