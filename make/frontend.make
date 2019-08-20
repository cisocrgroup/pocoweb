INSTALL_FRONTEND_DIR ?= /srv/pocoweb/www-data
PUBLIC_HTML = frontend/public_html
FE_FILES += $(PUBLIC_HTML)/doc.html
FE_FILES += $(PUBLIC_HTML)/index.html
FE_FILES += $(PUBLIC_HTML)/assets/js/vendor/rangy-core.js
FE_FILES += $(PUBLIC_HTML)/assets/js/require_main.built.js
FE_FILES += $(shell find $(PUBLIC_HTML)/assets -type f -name '*.png' -o -name '*.ico')
FE_CSS_FILES = $(shell find $(PUBLIC_HTML)/assets -type f -name '*.css')
FE_APP_FILES += $(shell find $(PUBLIC_HTML)/assets -type f -name '*.js' -o -name '*.tpl' | grep -v built)

frontend: $(FE_FILES)

$(PUBLIC_HTML)/assets/images/doc/overview.png: $(PUBLIC_HTML)/assets/images/doc/overview.dot
	$(call ECHO,$@)
	cat $< | dot -T png > $@

$(PUBLIC_HTML)/doc.html: $(PUBLIC_HTML)/doc.md $(PUBLIC_HTML)/assets/images/doc/overview.png
	$(call ECHO,$@)
	$V bash misc/scripts/md2html.sh $< | sed -e 's#<br>##g' > $@

$(PUBLIC_HTML)/assets/js/require_main.built.js: $(FE_APP_FILES) $(FE_CSS_FILES)
	$(call ECHO,$@)
	$V cd $(PUBLIC_HTML)/assets/js && node r.js -o build.js

.PHONY: install-frontend
install-frontend: $(FE_FILES) $(FE_CSS_FILES)
	$(call ECHO,$@)
	@for i in $^; do\
		install -D -m 644 $$i $(INSTALL_FRONTEND_DIR)$${i/frontend/}; done
