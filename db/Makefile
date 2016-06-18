include ../config.mk
SHELL = /bin/bash

default: .create-users .create-tables .create-books
.create-users: .create-tables ../config.mk
	@for entry in $(PCW_DEF_USERS); do\
		../rest/pocoweb_create_user ../rest/config.ini $${entry//,/ };\
	done
	@touch $@

.create-tables: tables.sql ../config.mk
	@mysql -h$(PCW_DB_HOST) -u$(PCW_DB_USER) --password=$(PCW_DB_PASS) < tables.sql
	@touch $@

.create-books: .create-users ../config.mk
	@for book in $(PCW_DEF_BOOKS); do\
		../rest/pocoweb_insert_book ../rest/config.ini $${book//,/ };\
	done
	@touch $@

clean:
	$(RM) .create-tables .create-users
install:
uninstall:
