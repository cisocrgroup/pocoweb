<?php
require_once(dirname(dirname(__FILE__)) . "/config.php");
require_once(LIBRARY_PATH . "/api.php");

function backend_get_api_version_route() {
	global $config;
	return $config["backend"]["url"] . $config["backend"]["routes"]["api_version"];
}

function backend_get_api_version() {
	$api = new Api(backend_get_api_version_route());
	$res = $api->get_request();
	if ($res !== NULL && array_key_exists("version", $res)) {
		return $res["version"];
	}
	error_log("Could not determine api-version");
	return "unknown";
}

function backend_get_login_route() {
	global $config;
	return $config["backend"]["url"] . $config["backend"]["routes"]["login"];
}

function backend_login($name, $pass) {
	$data = 'name=' . urlencode($name) . '&pass=' . urlencode($pass);
	// $data = ['name' => $name, 'pass' => $pass];
	$api = new Api(backend_get_login_route());
	$api->post_request($data);
	return $api->get_http_status_code();
}

function backend_is_logged_in() {
	global $config;
	return isset($_COOKIE[$config["cookies"]["sid"]]);
}

function backend_get_login_name() {
	global $config;
	if (isset($_COOKIE[$config["cookies"]["name"]])) {
		return $_COOKIE[$config["cookies"]["name"]];
	}
	return "unkown";
}

function backend_get_projects_route() {
	global $config;
	return $config["backend"]["url"] . $config["backend"]["routes"]["get_projects"];
}

function backend_get_projects() {
	$api = new Api(backend_get_projects_route());
	return $api->get_request();
}

function backend_get_first_page_route($pid) {
	global $config;
	return sprintf($config["backend"]["url"] . $config["backend"]["routes"]["get_first_page"], $pid);
}

function backend_get_page($pid, $p, $n) {
	if ($p === "first") {
		return backend_get_first_page($pid);
	}
	return NULL;
}

function backend_get_first_page($pid) {
	$api = new Api(backend_get_first_page_route($pid));
	return $api->get_request();
}
?>
