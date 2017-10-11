<?php
require_once("../resources/config.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");

global $user;
if ($user !== NULL) {
	require(TEMPLATES_PATH . "/header.php");
	frontend_render_success_div("You are logged in as $user");
} else if (isset($_POST["name"]) && isset($_POST["pass"])) {
	if (headers_sent()) {
		frontend_render_error_div("Could not set session cookies");
	}
	$api = backend_login($_POST["name"], $_POST["pass"]);
	$status = $api->get_http_status_code();
	switch ($status) {
	case "200":
		backend_set_session_cookie($api->get_response()["sid"]);
		backend_set_global_session_id();
		backend_set_global_user();
		require(TEMPLATES_PATH . "/header.php");
		frontend_render_success_div("You have successfully logged in");
		break;
	case "403":
		$SID = "";
		require(TEMPLATES_PATH . "/header.php");
		frontend_render_warning_div(
			"Login error: invalid username or password");
		frontend_render_login_div();
		break;
	default:
		$SID = "";
		require(TEMPLATES_PATH . "/header.php");
		frontend_render_error_div("Login error: " . $status);
		frontend_render_login_div();
		break;
	}
} else {
	require(TEMPLATES_PATH . "/header.php");
	frontend_render_login_div("Internal error: bad request to login.php");
}
require_once(TEMPLATES_PATH . "/footer.php");
?>
