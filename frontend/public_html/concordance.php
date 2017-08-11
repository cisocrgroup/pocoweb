<?php
require_once("../resources/config.php");
require_once(TEMPLATES_PATH . "/header.php");
require_once(LIBRARY_PATH . "/frontend.php");
if (!isset($_GET["q"]) && !isset($_GET["pid"])) {
	frontend_render_error_div(
		"Internal error: invalid or missing parameters");
} else {
	frontend_render_concordance_div(
		$_GET["pid"], $_GET["q"], isset($_GET["error-pattern"]));
}
require_once(TEMPLATES_PATH . "/footer.php");
?>
