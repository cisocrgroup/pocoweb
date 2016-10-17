CXXFLAGS := $(CXXFLAGS) -MP -MD
CXXFLAGS += -I../../modules/crow/include -I../../rest/src/core -fpic

all: simple-login.so
OBJS += simple-login.o
OBJS += LoginRoute.o
simple-login.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS)
clean:
	$(RM) *.so *.o *.d
-include $(patsubst %.o,%.d,$(OBJS))
