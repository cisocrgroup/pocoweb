<?php
require_once("../resources/config.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");
require_once(TEMPLATES_PATH . "/header.php");

if (isset($_GET["upload"])) {
	// frontend_upload_project($_POST);
	frontend_render_info_div("uploading upload=$_GET[upload]");
	foreach ($_POST as $key => $val) {
		echo("<p>$key = $val</p>");
	}
	foreach ($_FILES as $key => $val) {
		foreach($val as $keykey => $valval) {
			echo "<p>$keykey = $valval</p>";
		}
	}
}

if (backend_is_logged_in()) {
	frontend_render_projects_div();
} else {
	frontend_render_info_div("Welcome to PoCoWeb. Please <a href='login.php'>login</a>");
}

require_once(TEMPLATES_PATH . "/footer.php");
?>
