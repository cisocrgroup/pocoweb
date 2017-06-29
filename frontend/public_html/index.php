<?php
require_once("../resources/config.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");
require_once(TEMPLATES_PATH . "/header.php");

if (isset($_GET["upload"])) {
	if (isset($_FILES["archive"])) {
		frontend_upload_project_archive($_POST, $_FILES["archive"]);
	} else {
		frontend_render_error_div("Could not upload project: missing file");
	}
}

if (backend_is_logged_in()) {
	frontend_render_projects_div();
} else {
	frontend_render_info_div("Welcome to PoCoWeb. Please <a href='login.php'>login</a>");
}

require_once(TEMPLATES_PATH . "/footer.php");
?>
