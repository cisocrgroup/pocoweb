PCW_FRONTEND_DIR ?= /usr/share/nginx/html

FE_FILES += $(PCW_FRONTEND_DIR)/public_html/about.php
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
FE_FILES += $(PCW_FRONTEND_DIR)/resources/config.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/library/api.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/library/backend.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/library/frontend.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/templates/footer.php
FE_FILES += $(PCW_FRONTEND_DIR)/resources/templates/header.php
FE_FILES += $(PCW_FRONTEND_DIR)/public_html/LICENSE

.SECONDEXPANSION:
$(PCW_FRONTEND_DIR)/%.js: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m0644 $< $@
$(PCW_FRONTEND_DIR)%.css: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m0644 $< $@
$(PCW_FRONTEND_DIR)%.html: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m0644 $< $@
$(PCW_FRONTEND_DIR)/public_html/LICENSE: LICENSE
	$(call ECHO,$@)
	$V install -d $(dir $@)
	$V install -m0644 $< $@
$(PCW_FRONTEND_DIR)%.php: frontend/$$(subst $(PCW_FRONTEND_DIR)/,,$$@)
	$(call ECHO,$@)
	$V php -l $< > /dev/null
	$V install -d $(dir $@)
	$V install -m0644 $< $@

frontend/public_html/js/config.js: frontend/resources/config.php frontend/resources/genconfig.js.php
	$(call ECHO,$@)
	$V php frontend/resources/genconfig.js.php > $@
install-frontend: $(FE_FILES)

.PHONY: install-frontend
