<?php
require_once("../resources/config.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");
require_once(TEMPLATES_PATH . "/header.php");

if (isset($_GET["upload"])) {
	/*foreach ($_POST as $key => $val) {
		frontend_render_info_div("$key = $val");
	}*/
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
	$pid = $_GET["pid"];
	$api = backend_assign_project($pid, $_POST["assign-user-id"]);
	$status = $api->get_http_status_code();
	if ($status != 200) {
		frontend_render_error_div("Could not assign project #$pid: $status");
	} else {
		frontend_render_success_div("Successfully assigned project #$pid");
	}
} else if (isset($_GET["remove"])) {
	$pid = $_GET["pid"];
	$api = backend_remove_project($pid);
	$status = $api->get_http_status_code();
	if ($status != 200) {
		frontend_render_error_div("Could not remove project #$pid: $status");
	} else {
		frontend_render_success_div("Successfully removed project #$pid");
	}
} else if (isset($_GET["download"])) {
	$pid = $_GET["pid"];
	$api = backend_download_project($pid);
	$status = $api->get_http_status_code();
	if ($status != 200) {
		frontend_render_error_div("Could not download project #$pid: $status");
	} else {
		$link = $api->get_response()["archive"];
		frontend_render_success_div("Successfully prepared project #$pid for download");
		frontend_render_info_div("You can download the <a type=\"application/zip\" " .
			"href=\"$link\" download>project</a> now");
	}
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
global $USER;
if ($USER !== NULL) {
	frontend_render_projects_div();
} else {
	frontend_render_info_div("Welcome to PoCoWeb. Please <a href='login.php'>login</a>");
}

require_once(TEMPLATES_PATH . "/footer.php");
?>
