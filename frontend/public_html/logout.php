<?php
require_once("../resources/config.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");

if (backend_logout() === FALSE) {
	require(TEMPLATES_PATH . "/header.php");
	frontend_render_error_div("could not log out");
} else {
	require(TEMPLATES_PATH . "/header.php");
	frontend_render_success_div("You successfully logged out. <a href=\"login.php\">Login</a> again.");
}
require_once(TEMPLATES_PATH . "/footer.php");
?>
