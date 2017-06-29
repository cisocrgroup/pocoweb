<?php
require_once("../resources/config.php");
require_once(TEMPLATES_PATH . "/header.php");
require_once(LIBRARY_PATH . "/frontend.php");
if (isset($_GET["p"]) && isset($_GET["pid"]) && isset($_GET["u"])) {
	frontend_render_page_view_div($_GET["pid"], $_GET["p"], $_GET["u"], $_POST);
} else {
	frontend_render_error_div("internal error: invalid or missing parameters");
}
require_once(TEMPLATES_PATH . "/footer.php");
?>
