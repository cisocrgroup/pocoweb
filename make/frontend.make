PCW_FRONTEND_DIR ?= /usr/share/nginx/html

FE_FILES += $(PCW_FRONTEND_DIR)/public_html/LICENSE
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/about.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/account.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/adaptive.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/api/api.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/api/api_controller.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/api/backend.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/api/cacert.pem
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/api/config.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/api/upload.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/api/utils.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/assets/js/build.js
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/concordance.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/css/pcw.css
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/doc.html
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/doc.md
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/documentation.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/button-concordance-correction.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/button-correct.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/button-create-user.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/button-delete-account.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/button-select-correction-suggestion.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/button-update-account-settings.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/button-upload.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/checkbox-admin.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-assign-to-user.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-download-project.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-download-project.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-list-adaptive-tokens.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-open-project.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-order-profile.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-remove-project.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-remove.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/doc/glyphicon-split-project.png
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/favicon.ico
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/img/logo.jpg
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/index.html
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
FE_FILES += $(PCW_FRONTEND_DIR)/resources/config.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/library/api.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/library/backend.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/library/frontend.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/library/utils.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/templates/footer.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/templates/header.php

$(PCW_FRONTEND_DIR)/public_html/doc.html: frontend/public_html/doc.md
	$(call ECHO,$@)
	$V bash misc/scripts/md2html.sh $< | sed -e 's#<br>##g' > $@

# TODO: not that nice
$(PCW_FRONTEND_DIR)/public_html/assets/js/build.js: frontend/public_html/assets/js/r.js
	cd frontend/public_html/assets/js && node r.js -o build.js
	cp -r frontend/public_html/assets $(PCW_FRONTEND_DIR)/public_html

.SECONDEXPANSION:
%.js: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
%.md: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
%.css: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
%.jpg: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
%.pem: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
%.png: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
%.ico: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
%.html: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
%/LICENSE: LICENSE
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m 644 $< $@
%.php: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V php -l $< > /dev/null
	$V install -d $(dir $@)
	$V install -m 644 $< $@
$(PCW_FRONTEND_DIR)/public_html/api/api.php: frontend/resources/library/api.php
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V cp $< $@
$(PCW_FRONTEND_DIR)/public_html/api/config.php: frontend/resources/config.php
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V cp $< $@

frontend/public_html/js/config.js: frontend/resources/config.php frontend/resources/genconfig.js.php
	$(call ECHO,$@)
	$V php frontend/resources/genconfig.js.php > $@
install-frontend: $(FE_FILES)

.PHONY: install-frontend
