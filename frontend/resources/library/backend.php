<?php
require_once(dirname(dirname(__FILE__)) . "/config.php");
require_once(LIBRARY_PATH . "/api.php");

function backend_get_api_version_route() {
	global $config;
	return $config["backend"]["url"] . $config["backend"]["routes"]["api_version"];
}

function backend_get_api_version() {
	$api = new Api(backend_get_api_version_route());
	$api->get_request();
	return $api;
}

function backend_get_login_route() {
	global $config;
	return $config["backend"]["url"] . $config["backend"]["routes"]["login"];
}

function backend_login($name, $pass) {
	$data = array("name" => $name, "pass" => $pass);
	$api = new Api(backend_get_login_route());
	$api->post_request($data);
	return $api;
}

function backend_get_logout_route() {
	global $config;
	return $config["backend"]["url"] . $config["backend"]["routes"]["logout"];
}

function backend_logout() {
	$api = new Api(backend_get_logout_route());
	$api->get_request();
	return $api;
}

function backend_get_login_name() {
	$api = new Api(backend_get_login_route());
	$api->get_request();
	return $api;
}

function backend_get_projects_route() {
	global $config;
	return $config["backend"]["url"] . $config["backend"]["routes"]["get_projects"];
}

function backend_get_projects() {
	$api = new Api(backend_get_projects_route());
	$api->get_request();
	return $api;
}

function backend_get_users_route() {
	global $config;
	return $config["backend"]["url"] . $config["backend"]["routes"]["get_users"];
}

function backend_get_users() {
	$api = new Api(backend_get_users_route());
	$api->get_request();
	return $api;
}

function backend_create_user($post) {
	if (isset($post["admin"]) && $post["admin"] == "on") {
		$post["admin"] = true;
	} else {
		$post["admin"] = false;
	}
	$api = new Api(backend_get_users_route());
	$api->post_request($post);
	return $api;
}

function backend_get_delete_user_route($uid) {
	global $config;
	return sprintf($config["backend"]["url"] . $config["backend"]["routes"]["delete_user"], $uid);
}

function backend_delete_user($uid) {
	$api = new Api(backend_get_delete_user_route($uid));
	$api->delete_request();
	return $api;
}

function backend_get_split_project_route($pid) {
	global $config;
	return sprintf($config["backend"]["url"] . $config["backend"]["routes"]["split_project"], $pid);
}

function backend_split_project($pid, $post) {
	$data = array("n" => $post["split-n"]);
	if (isset($post["random"]) && $post["random"] == "on") {
		$data["random"] = true;
	} else {
		$data["random"] = false;
	}
	$api = new Api(backend_get_split_project_route($pid));
	$api->post_request($data);
	return $api;
}

function backend_get_nth_page_route($pid, $p) {
	global $config;
	return sprintf($config["backend"]["url"] . $config["backend"]["routes"]["get_nth_page"], $pid, $p);
}

function backend_get_last_page_route($pid) {
	global $config;
	return sprintf($config["backend"]["url"] . $config["backend"]["routes"]["get_last_page"], $pid);
}

function backend_get_first_page_route($pid) {
	global $config;
	return sprintf($config["backend"]["url"] . $config["backend"]["routes"]["get_first_page"], $pid);
}

function backend_get_correct_line_route($pid, $p, $lid) {
	global $config;
	return sprintf($config["backend"]["url"] . $config["backend"]["routes"]["correct_line"], $pid, $p, $lid);
}

function backend_correct_line($pid, $p, $lid, $d) {
	$data = array('d' => $d);
	$api = new Api(backend_get_correct_line_route($pid, $p, $lid));
	$api->post_request($data);
	return $api;
}

function backend_get_page($pid, $p) {
	if ($p === "first") {
		$api = new Api(backend_get_first_page_route($pid));
		$api->get_request();
		return $api;
	} else if ($p == "last") {
		$api = new Api(backend_get_last_page_route($pid));
		$api->get_request();
		return $api;
	} else {
		$api = new Api(backend_get_nth_page_route($pid, $p));
		$api->get_request();
		return $api;
	}
}

function backend_get_upload_project_route($file) {
	global $config;
	return $config["backend"]["url"] .
		$config["backend"]["routes"]["upload_project"] .
		"?file=" . urlencode($file);
}

function backend_upload_project($post, $name, $file) {
	$api = new Api(backend_get_upload_project_route($file));
	$api->post_request($post);
	return $api;
}
?>
