<?php
require_once("../resources/config.php");
require_once(LIBRARY_PATH . "/frontend.php");
require_once(LIBRARY_PATH . "/backend.php");
require_once(TEMPLATES_PATH . "/header.php");
global $user;
if (isset($_GET["create"])) {
	backend_create_new_user($_POST);
	frontend_render_success_div("Successfully created new user");
} else if (isset($_GET["delete"]) and isset($_GET["uid"])) {
	backend_delete_user($_GET["uid"]);
	frontend_render_success_div("Successfully deleted user id: $_GET[uid]");
}
if ($user === NULL) {
	frontend_render_info_div("Welcome to PoCoWeb. Please <a href='login.php'>login</a>");
} else if (!$user["admin"]) {
	frontend_render_info_div("You are not allowed to do user administration");
} else {
	frontend_render_users_div();
}
require_once(TEMPLATES_PATH . "/footer.php");
?>
