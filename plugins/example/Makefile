CXXFLAGS := $(CXXFLAGS) -MP -MD
CXXFLAGS += -I../../modules/crow/include -I../../rest/src/core -fpic

all: example.so
OBJS += example.o
example.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $< $(LDFLAGS)
clean:
	$(RM) *.so *.o *.d
-include $(patsubst %.o,%.d,$(OBJS))
