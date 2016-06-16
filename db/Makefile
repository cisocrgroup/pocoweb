include ../config.mk
SHELL = /bin/bash

default: .create-users
.create-users: .create-tables
	@for entry in $(PCW_DEF_USERS); do\
		../rest/pocoweb_create_user ../rest/config.ini $${entry//,/ };\
	done
	@touch $@

.create-tables: tables.sql
	@mysql -h$(PCW_DB_HOST) -u$(PCW_DB_USER) --password=$(PCW_DB_PASS) < tables.sql
	@touch $@

clean:
	$(RM) .create-tables .create-users
install:
uninstall:
