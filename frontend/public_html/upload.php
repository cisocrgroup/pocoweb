<?php
require_once("../resources/config.php");
require_once(TEMPLATES_PATH . "/header.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");
foreach ($_POST as $key => $val) {
	echo("<p>$key = $val</p>");
}
require_once(TEMPLATES_PATH . "/footer.php");
?>
