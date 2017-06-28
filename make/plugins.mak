PEX_OBJS += plugins/example/example.o

PSLOG_OBJS += plugins/simple-login/CreateUser.o
PSLOG_OBJS += plugins/simple-login/DeleteUser.o
PSLOG_OBJS += plugins/simple-login/Login.o
PSLOG_OBJS += plugins/simple-login/Logout.o
PSLOG_OBJS += plugins/simple-login/UpdateUser.o
PSLOG_OBJS += plugins/simple-login/simple-login.o

PPROF_OBJS += plugins/profiler/profiler.o
PPROF_OBJS += plugins/profiler/Config.o
PPROF_OBJS += plugins/profiler/LocalProfiler.o
PPROF_OBJS += plugins/profiler/Profiler.o
PPROF_OBJS += plugins/profiler/RemoteProfiler.o
PPROF_OBJS += plugins/profiler/ProfilerRoute.o

PLUGINS += lib/example.so
PLUGINS += lib/simple-login.so
PLUGINS += lib/profiler.so

lib/example.so: $(PEX_OBJS) | $(MODS) $(VENDS) $(LIBS) mkdir-lib
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS)
lib/simple-login.so: $(PSLOG_OBJS) | $(MODS) $(VENDS) $(LIBS) mkdir-lib
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS)
lib/profiler.so: $(PPROF_OBJS) | $(MODS) $(VENDS) $(LIBS) mkdir-lib
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS)

DEPS += $(patsubst %.o,%.d,$(PPROF_OBJS) $(PEX_OBJS) $(PSLOG_OBJS))
ALL += $(PLUGINS)
