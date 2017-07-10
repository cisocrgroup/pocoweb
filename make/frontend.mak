PCW_FRONTEND_DIR ?= /usr/share/nginx/html
install-frontend:
	cp -r frontend/public_html/ $(PCW_FRONTEND_DIR)
	cp -r frontend/resources/   $(PCW_FRONTEND_DIR)

.PHONY: install-frontend
