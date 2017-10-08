PCW_FRONTEND_DIR ?= /usr/share/nginx/html

FE_FILES += $(PCW_FRONTEND_DIR)/public_html/about.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/doc.html
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/adaptive.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/concordance.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/css/pcw.css
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/documentation.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/index.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/info.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/js/api.js
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/js/config.js
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/js/pcw.js
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/login.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/logout.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/page.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/upload.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/users.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/logo.jpg
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-remove.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-download-project.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-open-project.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-remove-project.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-split-project.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-list-adaptive-tokens.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-download-project.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-assign-to-user.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-order-profile.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/button-create-user.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/button-correct.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/button-upload.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/button-select-correction-suggestion.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/button-concordance-correction.png
FE_FILES += $(PCW_FRONTEND_DIR)/resources/config.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/library/api.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/library/backend.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/library/frontend.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/templates/footer.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/templates/header.php

$(PCW_FRONTEND_DIR)/public_html/doc.html: frontend/public_html/doc.md
	$(call ECHO,$@)
	$V bash misc/scripts/md2html.sh $< | sed -e 's#<br>##g' > $@

.SECONDEXPANSION:
$(PCW_FRONTEND_DIR)/%.js: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
$(PCW_FRONTEND_DIR)%.css: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
$(PCW_FRONTEND_DIR)%.jpg: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
$(PCW_FRONTEND_DIR)%.png: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
$(PCW_FRONTEND_DIR)%.html: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
$(PCW_FRONTEND_DIR)%.php: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V php -l $< > /dev/null
	$V install -d $(dir $@)
	$V install -m 644 $< $@

frontend/public_html/js/config.js: frontend/resources/config.php frontend/resources/genconfig.js.php
	$(call ECHO,$@)
	$V php frontend/resources/genconfig.js.php > $@
install-frontend: $(FE_FILES)

.PHONY: install-frontend
