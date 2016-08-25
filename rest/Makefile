#CXXFLAGS ?= -Wall -Werror -std=gnu++14 -g -Og
CXXFLAGS := $(CXXFLAGS) -MP -MD
CXXFLAGS += -I src
CXXFLAGS += -I gsm/Simple-Web-Server
CXXFLAGS += -I gsm/pugixml/src
CXXFLAGS += -I gsm/json/src
CXXFLAGS += -DBOOST_LOG_DYN_LINK
CXXFLAGS += -DPCW_API_VERSION=\"$(PCW_API_VERSION)\"

LDFLAGS ?=
LDFLAGS := $(LDFLAGS)
LDFLAGS += -l mysqlcppconn
LDFLAGS += -l ssl
LDFLAGS += -l lept
LDFLAGS += -l pthread
LDFLAGS += -l boost_regex
LDFLAGS += -l boost_system
LDFLAGS += -l boost_coroutine
LDFLAGS += -l boost_filesystem
LDFLAGS += -l boost_log
LDFLAGS += -l boost_log_setup
LDFLAGS += -l boost_thread
LDFLAGS += -l crypto

VPATH = src/db src/util src/api src/doc gsm/pugixml/src src/ex src 

OBJS += api.o
OBJS += Config.o
OBJS += DbTableUsers.o
OBJS += DbTableBooks.o
OBJS += BookDir.o
OBJS += Books.o
OBJS += Ex.o
## OBJS += GetPage.o
OBJS += User.o
OBJS += Page.o
OBJS += Book.o
OBJS += BookData.o
OBJS += Line.o
OBJS += Box.o
OBJS += Sessions.o
OBJS += db.o
OBJS += hash.o
OBJS += hocr.o
OBJS += pugixml.o

MAINS += pcwd.o
MAINS += pocoweb_create_user.o
MAINS += pocoweb_insert_book.o
MAINS += pcw_ex.o

default: pcwd pocoweb_insert_book pocoweb_create_user pcw_ex 

pcwd: pcwd.o libpocoweb.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

pocoweb_create_user: pocoweb_create_user.o libpocoweb.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

pocoweb_insert_book: pocoweb_insert_book.o libpocoweb.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

pcw_ex: pcw_ex.o libpocoweb.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

libpocoweb.a: $(OBJS)
	$(AR) rcs $@ $^

.PHONY: clean
clean:
	$(RM) *.o *.d libpocoweb.a pcwd pocweb_create_user pocoweb_insert_book pcw_ex

DEPS = $(patsubst %.o,%.d,$(OBJS) $(MAINS))
-include $(DEPS)

