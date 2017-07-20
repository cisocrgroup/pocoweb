<?php
require_once("../resources/config.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");
require(TEMPLATES_PATH . "/header.php");

$api = backend_logout();
$status = $api->get_http_status_code();
if ($status != 200) {
	frontend_render_warning_div("Error: could not log out: $status");
} else {
	frontend_render_success_div("You successfully logged out. <a href=\"login.php\">Login</a> again.");
}
require_once(TEMPLATES_PATH . "/footer.php");
?>
