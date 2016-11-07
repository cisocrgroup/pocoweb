default: .create-tables

.PHONY: clean install uninstall
clean:
	$(RM) .create-tables 
install:
uninstall:
test:

DB_USER := $(shell git config -f ../config.ini --get db.user)
DB_PASS := $(shell git config -f ../config.ini --get db.pass)
DB_HOST := $(shell git config -f ../config.ini --get db.host)

.create-tables: tables.sql ../config.ini
	@mysql -h$(DB_HOST) -u$(DB_USER) --password=$(DB_PASS) < tables.sql
	@touch $@

