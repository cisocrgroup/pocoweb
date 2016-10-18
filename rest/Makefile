CXXFLAGS ?= -Wall -Werror -std=gnu++14 -g
CXXFLAGS := $(CXXFLAGS) -MP -MD
CXXFLAGS += -fpic
CXXFLAGS += -I src/core
CXXFLAGS += -I ../modules/crow/include
CXXFLAGS += -I ../modules/pugixml/src
CXXFLAGS += -DPCW_API_VERSION=\"$(PCW_API_VERSION)\"
CXXFLAGS += -DBOOST_LOG_DYN_LINK

LDFLAGS ?=
LDFLAGS := $(LDFLAGS)
LDFLAGS += -L.
LDFLAGS += -ldl
# LDFLAGS += -lmysqlcppconn
LDFLAGS += -lssl
# LDFLAGS += -l lept
LDFLAGS += -lpthread
LDFLAGS += -lboost_log
# LDFLAGS += -lboost_regex
LDFLAGS += -lboost_system
# LDFLAGS += -lboost_coroutine
# LDFLAGS += -lboost_filesystem
# LDFLAGS += -lboost_log
# LDFLAGS += -lboost_log_setup
# LDFLAGS += -lboost_thread
LDFLAGS += -lcrypto

#VPATH = src/db src/util src/api src/doc gsm/pugixml/src src/ex src
VPATH = src src/core

OBJS += Config.o
OBJS += Database.o
OBJS += App.o
OBJS += ApiVersion.o
OBJS += User.o
OBJS += Sessions.o
OBJS += util.o
OBJS += db.o

## OBJS += api.o
## OBJS += Config.o
## OBJS += DbTableUsers.o
## OBJS += BookDir.o
## OBJS += Books.o
## OBJS += Ex.o
## ## OBJS += GetPage.o
## OBJS += User.o
## OBJS += Page.o
## OBJS += Book.o
## OBJS += BookData.o
## OBJS += Line.o
## OBJS += Box.o
## OBJS += Sessions.o
## OBJS += db.o
## OBJS += hash.o
## OBJS += hocr.o
## OBJS += pugixml.o

## MAINS += pcwd.o
## MAINS += pocoweb_create_user.o
## MAINS += pocoweb_insert_book.o
## MAINS += pcw_ex.o


default: all
all: pcwd

pcwd: pcwd.o libpcwcore.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

libpcwcore.a: $(OBJS)
	$(AR) rcs $@ $^

## libpcwcore.so: $(OBJS)
## 	$(CXX) $(CXXFLAGS) -shared -o $@ $^

.PHONY: clean
clean:
	$(RM) *.o *.d *.so *.a pcwd

DEPS = $(patsubst %.o,%.d,$(OBJS) pcwd.o)
-include $(DEPS)

