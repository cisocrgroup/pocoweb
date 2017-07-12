PCW_FRONTEND_DIR ?= /usr/share/nginx/html
frontend/public_html/js/config.js: frontend/resources/config.php frontend/resources/genconfig.js.php
	php frontend/resources/genconfig.js.php > $@
install-frontend: frontend/public_html/js/config.js
	cp -r frontend/public_html/ $(PCW_FRONTEND_DIR)
	cp -r frontend/resources/   $(PCW_FRONTEND_DIR)

.PHONY: install-frontend
