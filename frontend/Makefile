INSTALL_DIR ?= /srv/pocoweb/www-data
JS_FILES += public_html/api_doc.html
JS_FILES += public_html/manual.html
JS_FILES += public_html/index.html
JS_FILES += public_html/assets/js/vendor/rangy-core.js
JS_FILES += public_html/assets/js/require_main.built.js
JS_FILES += public_html/assets/images/doc/overview.svg
JS_FILES += $(shell find public_html -type f -name '*.png' -o -name '*.ico')
CSS_FILES = $(shell find public_html -type f -name '*.css')
APP_FILES += $(shell find public_html -type f -name '*.js' -o -name '*.tpl' | grep -v built)

.PHONY: all
all: $(JS_FILES)

public_html/assets/images/doc/overview.svg: public_html/assets/images/doc/overview.dot
	$(call ECHO,$@)
	cat $< | dot -T svg > $@

public_html/doc.html: public_html/doc.md public_html/assets/images/doc/overview.svg
	$(call ECHO,$@)
	$V bash data/scripts/md2html.sh $< | sed -e 's#<br>##g' > $@

public_html/assets/js/require_main.built.js: $(APP_FILES)
	$(call ECHO,$@)
	$V cd public_html/assets/js && node r.js -o build.js

.PHONY: install
install: $(JS_FILES) $(CSS_FILES)
	$(call ECHO,$@)
	@for i in $^; do install -v -D -m 644 $$i $(INSTALL_DIR)/$$i; done

.PHONY: clean
clean:
	$(RM) public_html/assets/js/require_main.built.js
