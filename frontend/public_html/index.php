<?php
require_once("../resources/config.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");
require_once(TEMPLATES_PATH . "/header.php");

if (isset($_GET["upload"])) {
	if (isset($_FILES["archive"])) {
		$_POST["uri"] = "";
		$_POST["description"] = "";
		frontend_upload_project_archive($_POST, $_FILES["archive"]);
	} else {
		frontend_render_error_div("Could not upload project: missing file");
	}
} else if (isset($_GET["split"])) {
	$pid = $_GET["pid"];
	$api = backend_split_project($pid, $_POST);
	$status = $api->get_http_status_code();
	if ($status != 200) {
		frontend_render_error_div("Could not split project #$pid: $status");
	} else {
		frontend_render_success_div("Successfully split project #$pid");
	}
} else if (isset($_GET["assign"])) {
	foreach ($_POST as $key => $val) {
		frontend_render_info_div("$key = $val");
	}
	$pid = $_GET["pid"];
	$api = backend_assign_project($pid, $_POST);
	$status = $api->get_http_status_code();
	if ($status != 200) {
		frontend_render_error_div("Could not assign project #$pid: $status");
	} else {
		frontend_render_success_div("Successfully assigned project #$pid");
	}
} else if (isset($_GET["remove"])) {
	frontend_render_error_div("Not implemented yet: remove project $_GET[pid]");
} else if (isset($_GET["download"])) {
	frontend_render_error_div("Not implemented yet: download project $_GET[pid]");
} else if (isset($_GET["finish"])) {
	$pid = $_GET["pid"];
	$api = backend_finish_project($pid);
	$status = $api->get_http_status_code();
	if ($status != 200) {
		frontend_render_error_div("Could not finish project #$pid: $status");
	} else {
		frontend_render_success_div("Successfully finished project #$pid");
	}
}
global $user;
if ($user !== NULL) {
	frontend_render_projects_div();
} else {
	frontend_render_info_div("Welcome to PoCoWeb. Please <a href='login.php'>login</a>");
}

require_once(TEMPLATES_PATH . "/footer.php");
?>
