<?php
require_once(dirname(dirname(__FILE__)) . "/config.php");
require_once(LIBRARY_PATH . "/api.php");

function backend_set_global_user() {
	global $USER;
	global $SID;
	if (strlen($SID) > 0) {
		$api = backend_get_login_name();
		if ($api->get_http_status_code() == 200) {
			$USER = $api->get_response();
		} else {
			$USER = NULL;
		}
	} else {
		$USER = NULL;
	}
}

function backend_set_global_session_id() {
	global $SID;
	$SID = backend_get_session_cookie();
}

function backend_set_global_api_version() {
	global $API;
	$api = backend_get_api_version();
	if ($api->get_http_status_code() == 200) {
		$API = $api->get_response()["version"];
	} else {
		$API = "";
	}
}

function backend_setup_globals() {
	backend_set_global_api_version();
	// session id must be set before the user.
	backend_set_global_session_id();
	backend_set_global_user();
}

function backend_get_api_version_route() {
	global $config;
	return $config["backend"]["url"] .
		$config["backend"]["routes"]["api_version"];
}

function backend_get_api_version() {
	$api = new Api(backend_get_api_version_route());
	$api->get_request();
	return $api;
}

function backend_get_login_route() {
	global $config;
	return $config["backend"]["url"] .
		$config["backend"]["routes"]["login"];
}

function backend_login($name, $pass) {
	$data = array("name" => $name, "pass" => $pass);
	$api = new Api(backend_get_login_route());
	$api->post_request($data);
	return $api;
}

function backend_set_session_cookie($sid) {
	global $config;
	$res = setcookie($config["cookies"]["sid"], $sid,
		time() + $config["cookies"]["expires"],
		"/", $config["cookies"]["domain"]);
	if (!$res) {
		error_log("(backend_set_session_cookie) " .
			"could not set cookie: $sid");
	}
}

function backend_get_session_cookie() {
	global $config;
	$res = "";
	if (isset($_COOKIE[$config["cookies"]["sid"]])) {
		$res = $_COOKIE[$config["cookies"]["sid"]];
	}
	return $res;

}

function backend_get_logout_route() {
	global $config;
	return $config["backend"]["url"] .
		$config["backend"]["routes"]["logout"];
}

function backend_logout() {
	$api = new Api(backend_get_logout_route());
	$api->set_session_id(backend_get_session_cookie());
	$api->get_request();
	return $api;
}

function backend_get_login_name() {
	global $SID;
	$api = new Api(backend_get_login_route());
	$api->set_session_id($SID);
	$api->get_request();
	return $api;
}

function backend_get_projects_route() {
	global $config;
	return $config["backend"]["url"] .
		$config["backend"]["routes"]["get_projects"];
}

function backend_get_projects() {
	$api = new Api(backend_get_projects_route());
	$api->get_request();
	return $api;
}

function backend_get_users_route() {
	global $config;
	return $config["backend"]["url"] .
		$config["backend"]["routes"]["get_users"];
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
	return sprintf($config["backend"]["url"] .
		$config["backend"]["routes"]["delete_user"], $uid);
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

function backend_get_assign_project_route($pid) {
	global $config;
	return sprintf($config["backend"]["url"] . $config["backend"]["routes"]["assign_project"], $pid);
}

function backend_assign_project($pid, $post) {
	$data = array("name" => $post["assign-user-name"]);
	$api = new Api(backend_get_assign_project_route($pid));
	$api->post_request($data);
	return $api;
}

function backend_get_finish_project_route($pid) {
	global $config;
	return sprintf($config["backend"]["url"] . $config["backend"]["routes"]["finish_project"], $pid);
}

function backend_finish_project($pid) {
	$data = array();
	$api = new Api(backend_get_finish_project_route($pid));
	$api->post_request($data);
	return $api;
}

function backend_get_remove_project_route($pid) {
	global $config;
	return sprintf($config["backend"]["url"] . $config["backend"]["routes"]["remove_project"], $pid);
}

function backend_remove_project($pid) {
	$api = new Api(backend_get_remove_project_route($pid));
	$api->delete_request();
	return $api;
}

function backend_get_download_project_route($pid) {
	global $config;
	return sprintf($config["backend"]["url"] . $config["backend"]["routes"]["download_project"], $pid);
}

function backend_download_project($pid) {
	$api = new Api(backend_get_download_project_route($pid));
	$api->get_request();
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
