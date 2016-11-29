CXXFLAGS := $(CXXFLAGS)
CXXFLAGS += -I../../modules/crow/include -I../../rest/src -fpic
LDFLAGS := $(LDFLAGS)
LDFLAGS += -lmysqlcppconn

all: profiler.so

OBJS += profiler.o
OBJS += Config.o
OBJS += ProfilerRoute.o

%.so: ../../rest/libpcwcor.a ../../rest/libpcwparser.a
%.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS)
clean:
	$(RM) *.so *.o *.d
-include $(patsubst %.o,%.d,$(OBJS))
