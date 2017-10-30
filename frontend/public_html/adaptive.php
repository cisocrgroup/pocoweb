<?php
require_once("../resources/config.php");
require_once(TEMPLATES_PATH . "/header.php");
require_once(LIBRARY_PATH . "/frontend.php");
if (!isset($_GET["pid"])) {
	frontend_render_error_div(
		"Internal error: invalid or missing parameters");
} else {
	frontend_render_adaptive_tokens_div($_GET["pid"]);
}
require_once(TEMPLATES_PATH . "/footer.php");
?>
