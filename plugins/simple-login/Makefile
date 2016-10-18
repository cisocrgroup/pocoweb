CXXFLAGS := $(CXXFLAGS)
CXXFLAGS += -I../../modules/crow/include -I../../rest/src/core -fpic

all: simple-login.so
OBJS += simple-login.o
OBJS += CreateUser.o
OBJS += UpdateUser.o
OBJS += DeleteUser.o
OBJS += Login.o

simple-login.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS)
clean:
	$(RM) *.so *.o *.d
-include $(patsubst %.o,%.d,$(OBJS))
