PEX_OBJS += plugins/example/example.o

PSLOG_OBJS += plugins/simple-login/DeleteUser.o
PSLOG_OBJS += plugins/simple-login/Login.o
PSLOG_OBJS += plugins/simple-login/Logout.o
PSLOG_OBJS += plugins/simple-login/UpdateUser.o
PSLOG_OBJS += plugins/simple-login/simple-login.o

PLUGINS += lib/example.so
PLUGINS += lib/simple-login.so

lib/example.so: $(PEX_OBJS) | $(MODS) $(VENDS) $(LIBS) mkdir-lib
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS)
lib/simple-login.so: $(PSLOG_OBJS) | $(MODS) $(VENDS) $(LIBS) mkdir-lib
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS)

DEPS += $(patsubst %.o,%.d,$(PEX_OBJS) $(PSLOG_OBJS))
ALL += $(PLUGINS)
