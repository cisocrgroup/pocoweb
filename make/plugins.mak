PEX_OBJS += plugins/example/example.o

PSLOG_OBJS += plugins/simple-login/CreateUser.o
PSLOG_OBJS += plugins/simple-login/DeleteUser.o
PSLOG_OBJS += plugins/simple-login/LoggedIn.o
PSLOG_OBJS += plugins/simple-login/Login.o
PSLOG_OBJS += plugins/simple-login/UpdateUser.o
PSLOG_OBJS += plugins/simple-login/simple-login.o

PPROF_OBJS += plugins/profiler/src/profiler.o
PPROF_OBJS += plugins/profiler/src/Config.o
PPROF_OBJS += plugins/profiler/src/LocalProfiler.o
PPROF_OBJS += plugins/profiler/src/Profiler.o
PPROF_OBJS += plugins/profiler/src/RemoteProfiler.o
PPROF_OBJS += plugins/profiler/src/ProfilerRoute.o

PLUGINS += example.so
PLUGINS += simple-login.so
PLUGINS += profiler.so

example.so: $(PEX_OBJS)
	$(CXX) $(CXXFLAGS) $(FPIC) -shared -o $@ $< $(LDFLAGS)
simple-login.so: $(PSLOG_OBJS)
	$(CXX) $(CXXFLAGS) $(FPIC) -shared -o $@ $< $(LDFLAGS)
profiler.so: $(PPROF_OBJS)
	$(CXX) $(CXXFLAGS) $(FPIC) -shared -o $@ $< $(LDFLAGS)

DEPS += $(patsubst %.o,%d,$(PPROF_OBJS) $(PEX_OBJS) $(PSLOG_OBJS))
ALL += $(PLUGINS)
