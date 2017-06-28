<?php
require_once("../resources/config.php");
require_once(TEMPLATES_PATH . "/header.php");
require_once(LIBRARY_PATH . "/frontend.php");
if (isset($_GET["p"]) && isset($_GET["pid"]) && isset($_GET["n"])) {
	// update lines to server
	foreach ($_POST as $key => $val) {
		if (preg_match('/(\d+)-(\d+)-(\d+)/', $key, $m)) {
			$status = backend_correct_line($m[1], $m[2], $m[3], $val);
			if ($status !== 200) {
				frontend_render_error_div(
					"Error updating line: $m[1]-$m[2]-$m[3] ($status)");
			} else {
				frontend_render_success_div(
					"Successfully updated line: $m[1]-$m[2]-$m[3] ($status)");
			}
		}
	}
	frontend_render_page_view_div($_GET["pid"], $_GET["p"], $_GET["n"]);
} else {
	frontend_render_error_div("internal error: invalid or missing parameters");
}
require_once(TEMPLATES_PATH . "/footer.php");
?>
