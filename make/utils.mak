# make/utils.mak

MAINS += pcwc
pcwc: utils/src/pcwc.cpp $(LIBS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) -lcurl
