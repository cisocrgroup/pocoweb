<?php
require_once("../resources/config.php");
require_once(TEMPLATES_PATH . "/header.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");

if (backend_is_logged_in()) {
	frontend_render_success_div("You logged in successfully");
} else if (isset($_POST["name"]) && isset($_POST["pass"])) {
	if (headers_sent()) {
		frontend_render_error_div("could not set session cookies");
	}
	$status = backend_login($_POST["name"], $_POST["pass"]);
	switch ($status) {
	case "200":
		frontend_render_success_div("You logged in successfully");
		break;
	case "403":
		frontend_render_warning_div(
			"login error: invalid username or password");
		frontend_render_login_div();
		break;
	default:
		frontend_render_error_div("login error: " . $status);
		frontend_render_login_div();
		break;
	}
} else {
	frontend_render_login_div();
}
require_once(TEMPLATES_PATH . "/footer.php");
?>
