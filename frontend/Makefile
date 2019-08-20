INSTALL_DIR ?= /srv/pocoweb/www-data
FE_FILES += public_html/doc.html
FE_FILES += public_html/index.html
FE_FILES += public_html/assets/js/vendor/rangy-core.js
FE_FILES += public_html/assets/js/require_main.built.js
FE_FILES += public_html/assets/images/doc/overview.svg
FE_FILES += $(shell find public_html -type f -name '*.png' -o -name '*.ico')
FE_CSS_FILES = $(shell find public_html -type f -name '*.css')
FE_APP_FILES += $(shell find public_html -type f -name '*.js' -o -name '*.tpl' | grep -v built)

.PHONY: all
all: $(FE_FILES)

public_html/assets/images/doc/overview.svg: public_html/assets/images/doc/overview.dot
	$(call ECHO,$@)
	cat $< | dot -T svg > $@

public_html/doc.html: public_html/doc.md public_html/assets/images/doc/overview.svg
	$(call ECHO,$@)
	$V bash data/scripts/md2html.sh $< | sed -e 's#<br>##g' > $@

public_html/assets/js/require_main.built.js: $(FE_APP_FILES)
	$(call ECHO,$@)
	$V cd public_html/assets/js && node r.js -o build.js

.PHONY: install
install: $(FE_FILES) $(FE_CSS_FILES)
	$(call ECHO,$@)
	@for i in $^; do install -v -D -m 644 $$i $(INSTALL_DIR)/$$i; done

.PHONY: clean
clean:
	$(RM) public_html/assets/js/require_main.built.js
