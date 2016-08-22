include ../config.mk

CXXFLAGS ?= -Wall -Werror -std=gnu++14 -g -Og
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
## OBJS += Login.o
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

MAINS += pocowebd.o
MAINS += pocoweb_create_user.o
MAINS += pocoweb_insert_book.o
MAINS += pcw_ex.o

default: pocowebd pocoweb_insert_book pocoweb_create_user pcw_ex config.ini

pocowebd: pocowebd.o libpocoweb.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

pocoweb_create_user: pocoweb_create_user.o libpocoweb.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

pocoweb_insert_book: pocoweb_insert_book.o libpocoweb.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

pcw_ex: pcw_ex.o libpocoweb.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

libpocoweb.a: $(OBJS)
	$(AR) rcs $@ $^

config.ini:
	@cp ../misc/default/config.def.ini $@
	@sed -i -e "s/@PCW_DB_USER@/$(PCW_DB_USER)/g" $@
	@sed -i -e "s/@PCW_DB_HOST@/$(PCW_DB_HOST)/g" $@
	@sed -i -e "s/@PCW_DB_PASS@/$(PCW_DB_PASS)/g" $@
	@sed -i -e "s/@PCW_DAEMON_HOST@/$(PCW_DAEMON_HOST)/g" $@
	@sed -i -e "s/@PCW_DAEMON_PORT@/$(PCW_DAEMON_PORT)/g" $@
	@sed -i -e "s/@PCW_DAEMON_THREADS@/$(PCW_DAEMON_THREADS)/g" $@
	@sed -i -e "s/@PCW_LOG_FILE@/$(PCW_LOG_FILE)/g" $@

.PHONY: clean
clean:
	$(RM) *.o *.d libpocoweb.a pocwebd pocweb_create_user pocoweb_insert_book pcw_ex config.ini

DEPS = $(patsubst %.o,%.d,$(OBJS) $(MAINS))
-include $(DEPS)

