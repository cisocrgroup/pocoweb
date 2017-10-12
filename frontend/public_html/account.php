<?php
require_once("../resources/config.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");
require_once(TEMPLATES_PATH . "/header.php");

global $USER;
if (isset($_GET["update"])) {
		if (!isset($_POST["name"]) || !isset($_POST["email"]) ||
				!isset($_POST["institute"]) || !isset($_POST["pass1"]) ||
				!isset($_POST["pass2"])) {
				frontend_render_error_div("invalid post parameters");
		} else if ($_POST["pass1"] != $_POST["pass2"]) {
				frontend_render_error_div("passwords do not match");
		} else {
				$api = backend_update_user($USER["id"], $_POST["name"], $_POST["email"],
						$_POST["institute"], $_POST["pass1"]);
				$status = $api->get_http_status_code();
				if ($status == 200) {
						$USER = $api->get_response();
						frontend_render_success_div(
								"successfully updated user account");
				} else {
						frontend_render_error_div("could not update user account: " .
								backend_get_http_status_info($status));
				}
		}
}
if (isset($_GET["delete"])) {
		$api = backend_delete_user($USER['id']);
		$status = $api->get_http_status_code();
		if ($status == 200) {
				$api = backend_logout(); // ignore errors.
				frontend_render_success_div(
						"You successfully deleted your user account. " .
								"<a href=\"login.php\">Login</a> again.");
		} else {
				frontend_render_error_div("could not delete user account: " .
						backend_get_http_status_info($status));
		}
} else if ($USER !== NULL) {
	frontend_render_account_div($USER);
} else {
	frontend_render_info_div("Welcome to PoCoWeb. Please <a href='login.php'>login</a>");
}

require_once(TEMPLATES_PATH . "/footer.php");
?>
