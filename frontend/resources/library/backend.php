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

function backend_get_logout_route() {
	global $config;
	return $config["backend"]["url"] . $config["backend"]["routes"]["logout"];
}

function backend_logout() {
	$api = new Api(backend_get_logout_route());
	$api->get_request();
}

function backend_is_logged_in() {
	return backend_get_login_name() !== NULL;
}

function backend_get_login_name() {
	global $config;
	$api = new Api(backend_get_login_route());
	return $api->get_request();
}

function backend_get_projects_route() {
	global $config;
	return $config["backend"]["url"] . $config["backend"]["routes"]["get_projects"];
}

function backend_get_projects() {
	$api = new Api(backend_get_projects_route());
	return $api->get_request();
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
	$data = 'd=' . urlencode($d);
	$api = new Api(backend_get_correct_line_route($pid, $p, $lid));
	$api->post_request($data);
	return $api->get_http_status_code();
}

function backend_get_page($pid, $p, $n) {
	if ($p === "first") {
		$api = new Api(backend_get_first_page_route($pid));
		return $api->get_request();
	} else if ($p == "last") {
		$api = new Api(backend_get_last_page_route($pid));
		return $api->get_request();
	} else {
		$api = new Api(backend_get_nth_page_route($pid, $p));
		return $api->get_request();
	}
}
?>
