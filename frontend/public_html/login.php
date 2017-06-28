<?php
require_once("../resources/config.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");

if (backend_is_logged_in()) {
	require(TEMPLATES_PATH . "/header.php");
	frontend_render_success_div("You logged in successfully");
} else if (isset($_POST["name"]) && isset($_POST["pass"])) {
	require(TEMPLATES_PATH . "/header.php");
	if (headers_sent()) {
		frontend_render_error_div("could not set session cookies");
	}
	$status = backend_login($_POST["name"], $_POST["pass"]);
	switch ($status) {
	case "200":
		frontend_render_success_div("You successfully logged in");
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
	require(TEMPLATES_PATH . "/header.php");
	frontend_render_login_div("internal error: bad request to login.php");
}
require_once(TEMPLATES_PATH . "/footer.php");
?>
