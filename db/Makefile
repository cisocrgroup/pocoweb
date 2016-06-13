include ../config.mk
SHELL = /bin/bash

default: .create-users
.create-users: .create-tables
	@for entry in $(PCW_DEF_USERS); do\
		../misc/scripts/pocoweb_db_create_user.sh \
			$(PCW_DB_USER) $(PCW_DB_HOST) $(PCW_DB_PASS) $${entry//,/ } 2>/dev/null;\
	done
	touch $@
.create-tables: tables.sql
	mysql -h$(PCW_DB_HOST) -u$(PCW_DB_USER) --password=$(PCW_DB_PASS) < tables.sql 2>/dev/null
	touch $@

clean:
	$(RM) .create-tables .create-users
install:
uninstall:
clean:
