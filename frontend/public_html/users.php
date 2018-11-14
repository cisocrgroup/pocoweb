<?php
require_once("../resources/config.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");
require_once(TEMPLATES_PATH . "/header.php");
if (isset($_GET["create"])) {
	if ($_POST["pass"] != $_POST["pass2"]) {
		frontend_render_error_div("Error: could not create user $_POST[name]: passwords do not match");
	} else {
        $admin = isset($_POST['admin']) && $_POST['admin'] == "on";
		$api = backend_create_user(
            $_POST['name'],
            $_POST['email'],
            $_POST['institute'],
            $admin,
            $_POST['pass']);
		$status = $api->get_http_status_code();
		if ($status == 200) {
			frontend_render_success_div("Successfully created new user");
		} else {
			frontend_render_error_div("Error: could not create user: $status");
		}
	}
} else if (isset($_GET["delete"]) and isset($_GET["uid"])) {
	backend_delete_user($_GET["uid"]);
	frontend_render_success_div("Successfully deleted user id: $_GET[uid]");
}
global $SID;
if ($SID === NULL) {
	frontend_render_info_div("Welcome to PoCoWeb. Please <a href='login.php'>login</a>");
} else if (!$SID["user"]["admin"]) {
	frontend_render_warning_div(
        "User ". $SID['user']['name'] . " (" . $SID["user"]["email"] .
        ") is not allowed to do user administration");
} else {
	frontend_render_users_div($SID['user']);
}
require_once(TEMPLATES_PATH . "/footer.php");
?>
