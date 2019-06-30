<?php
require_once("config.php");
require_once("api.php");

function backend_set_global_user() {
	global $USER;
	global $SID;
	// if (strlen($SID) > 0) {
	// 	$api = backend_get_login_name();
	// 	if ($api->get_http_status_code() == 200) {
	// 		$USER = $api->get_response()["user"];
	// 	} else {
	// 		$USER = NULL;
	// 	}
	// } else {
	// 	$USER = NULL;
	// }
}

function backend_get_api_version_route() {
	global $config;
	return $config["backend"]["internalURL"] .
		$config["backend"]["routes"]["apiVersion"];
}


function backend_set_global_session_id() {
	global $SID;
	$SID = backend_get_session_cookie();
}

function backend_get_login_route() {
	global $config;
	return $config["backend"]["internalURL"] .
		$config["backend"]["routes"]["login"];
}

function backend_get_http_status_info($status) {
	global $config;
	if (isset($config["httpStatusInfo"][$status])) {
		return "$status " . $config["httpStatusInfo"][$status];
	}
	return "$status Unknown";
}

function backend_get_upload_error_info($code) {
    global $uploadErrorCodes;
    if (isset($uploadErrorCodes[$code])) {
        return "$uploadErrorCodes[$code]";
    }
    return "unknown upload error";
}

function backend_login($email, $pass) {
	$data = array("email" => $email, "password" => $pass);
	$api = new Api(backend_get_login_route());
	$api->post_request($data);
	return $api;
}

function backend_set_session_cookie($session) {
	global $config;
    $json = json_encode($session);
	$res = setcookie(
        $config["cookies"]["sid"],
        $json,
        time() + $config["cookies"]["expires"]);
	if (!$res) {
		error_log("(backend_set_session_cookie) " .
			"could not set cookie: $sid");
	}
}

function backend_get_session_cookie() {
		//print_r($_COOKIE);
	global $config;
	$res = "";
	if (!isset($_COOKIE[$config["cookies"]["sid"]])) {
        return NULL;
    }
    $res = $_COOKIE[$config["cookies"]["sid"]];
    return json_decode($res, TRUE);
}

function backend_get_logout_route() {
	global $config;
	return $config["backend"]["internalURL"] .
		$config["backend"]["routes"]["logout"];
}

function backend_logout() {
	global $SID;
	$api = new Api(backend_get_logout_route());
	$api->set_session_id($SID);
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
	return $config["backend"]["internalURL"] .
		$config["backend"]["routes"]["get_projects"];
}

function backend_get_projects() {
	$api = new Api(backend_get_projects_route());
	global $SID;
	$api->set_session_id($SID);
	$api->get_request();
	return $api;
}

function backend_get_users_route() {
	global $config;
	return $config["backend"]["internalURL"] .
		$config["backend"]["routes"]["getUsers"];
}

function backend_get_users() {
	$api = new Api(backend_get_users_route());
	global $SID;
	$api->set_session_id($SID);
	$api->get_request();
	return $api;
}

function backend_create_user($name, $email, $institute, $admin, $pass) {
    $data = array(
        'user' => array(
            'name' => $name,
            'email' => $email,
            'institute' => $institute,
            'admin' => $admin,
        ),
        'password' => $pass,
    );
	$api = new Api(backend_get_users_route());
	global $SID;
	$api->set_session_id($SID);
	$api->post_request($data);
	return $api;
}

function backend_get_delete_user_route($uid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["deleteUser"], $uid);
}

function backend_delete_user($uid) {
	$api = new Api(backend_get_delete_user_route($uid));
	global $SID;
	$api->set_session_id($SID);
	$api->delete_request();
	return $api;
}

function backend_get_split_project_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
	  $config["backend"]["routes"]["splitProject"], $pid);
}

function backend_split_project($pid, $post) {
	$data = array("n" => $post["split-n"]);
	if (isset($post["random"]) && $post["random"] == "on") {
		$data["random"] = true;
	} else {
		$data["random"] = false;
	}
	$api = new Api(backend_get_split_project_route($pid));
	global $SID;
	$api->set_session_id($SID);
	$api->post_request($data);
	return $api;
}

function backend_get_assign_project_route($pid, $uid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
	  $config["backend"]["routes"]["assignProject"], $pid, $uid);
}

function backend_assign_project($pid, $uid) {
	global $SID;
	$api = new Api(backend_get_assign_project_route($pid, $uid));
	$api->set_session_id($SID);
	$api->get_request();
	return $api;
}

function backend_get_finish_project_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
	  $config["backend"]["routes"]["finishProject"], $pid);
}

function backend_finish_project($pid) {
	$data = array();
	$api = new Api(backend_get_finish_project_route($pid));
	global $SID;
	$api->set_session_id($SID);
	$api->post_request($data);
	return $api;
}

function backend_get_project_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
	  $config["backend"]["routes"]["getProject"], $pid);
}

function backend_remove_project($pid) {
	$api = new Api(backend_get_project_route($pid));
	global $SID;
	$api->set_session_id($SID);
	$api->delete_request();
	return $api;
}

function backend_get_download_project_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
	  $config["backend"]["routes"]["downloadProject"], $pid);
}

function backend_download_project($pid) {
	$api = new Api(backend_get_download_project_route($pid));
	global $SID;
	$api->set_session_id($SID);
	$api->get_request();
	return $api;
}



function backend_get_nth_page_route($pid, $p) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
	  $config["backend"]["routes"]["getNthPage"], $pid, $p);
}

function backend_get_last_page_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
	  $config["backend"]["routes"]["getLastPage"], $pid);
}

function backend_get_first_page_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
	  $config["backend"]["routes"]["getFirstPage"], $pid);
}

function backend_get_correct_line_route($pid, $p, $lid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["correctLine"], $pid, $p, $lid);
}

function backend_correct_line($pid, $p, $lid, $d) {
	$data = array(
		'correction' => $d,
	);
	$api = new Api(backend_get_correct_line_route($pid, $p, $lid));
	global $SID;
	$api->set_session_id($SID);
	$api->post_request($data);
	return $api;
}

function backend_get_correct_word_route($pid, $p, $lid,$tid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["correctWord"], $pid, $p, $lid,$tid);
}

function backend_correct_word($pid, $p, $lid, $tid, $d) {
	$data = array(
		'correction' => $d,
	);
	$api = new Api(backend_get_correct_word_route($pid, $p, $lid,$tid));
	global $SID;
	$api->set_session_id($SID);
	$api->post_request($data);
	return $api;
}



function backend_get_page($pid, $p) {
	if ($p === "first") {
		$api = new Api(backend_get_first_page_route($pid));
		$api->set_session_id($SID);
		$api->get_request();
		return $api;
	} else if ($p == "last") {
		$api = new Api(backend_get_last_page_route($pid));
		global $SID;
		$api->set_session_id($SID);
		$api->get_request();
		return $api;
	} else {
		$api = new Api(backend_get_nth_page_route($pid, $p));
		global $SID;
		$api->set_session_id($SID);
		$api->get_request();
		return $api;
	}
}

function backend_get_upload_project_route($file) {
	global $config;
	return $config["backend"]["internalURL"] .
		$config["backend"]["routes"]["uploadProject"];
}

function backend_upload_project($post, $name, $file) {
	$post["file"] = $file;
	$api = new Api(backend_get_upload_project_route($file));
	global $SID;
	$api->set_session_id($SID);
	$api->post_request($post);
	return $api;
}

function backend_get_search_route($pid, $q, $p) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["search"],
		$pid, urlencode($q), (int)$p);
}

function backend_get_suggestions_route($pid, $q) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["searchSuggestions"],
		$pid,urlencode($q));
}

function backend_get_all_suggestions_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["getAllSuggestions"],
		$pid);
}

function backend_get_error_patterns_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["getErrorPatterns"],
		$pid);
}

function backend_get_suspicious_words_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["getSuspiciousWords"],
		$pid);
}



function backend_get_order_profile_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["orderProfile"], $pid);
}

function backend_get_concordance($pid, $q, $isErrorPattern) {
	global $SID;
	$api = new Api(backend_get_search_route($pid, $q, $isErrorPattern));
	$api->set_session_id($SID);
	$api->get_request();
	return $api;
}

function backend_get_split_images_route($pid, $p, $lid, $tid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["splitImages"],
		$pid, $p, $lid, $tid);
}

function backend_get_split_images($word) {
	global $SID;
	$api = new Api(backend_get_split_images_route(
		$word["projectId"], $word["pageId"],
		$word["lineId"], $word["tokenId"]));
	$api->set_session_id($SID);
	$api->get_request();
	return $api;
}

function backend_get_languages_route($url) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["languages"], $url);
}

function backend_get_languages() {
	global $SID;
	$api = new Api(backend_get_languages_route("local"));
	$api->set_session_id($SID);
	$api->get_request();
	return $api;
}

function backend_get_adaptive_tokens_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["adaptiveTokens"], $pid);
}

function backend_get_adaptive_tokens($pid) {
	global $SID;
	$api = new Api(backend_get_adaptive_tokens_route($pid));
	$api->set_session_id($SID);
	$api->get_request();
	return $api;
}

function backend_get_update_user_route($uid) {
		global $config;
		return sprintf($config["backend"]["internalURL"] .
				$config["backend"]["routes"]["updateUser"], $uid);
}

function backend_update_user($uid, $name, $email, $institute, $pass) {
    $data = array(
        'user' => array (
            'id' => $uid,
            'name' => $name,
            'email' => $email,
            'institute' => $institute,
        ),
    );
    if ($pass != "") {
        $data['password'] = $pass;
    }
	global $SID;
	$api = new Api(backend_get_update_user_route($uid));
    $api->set_session_id($SID);
    $api->put_request($data);
    return $api;
}

function backend_get_start_el_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["startEL"],$pid);
}
function backend_get_inspect_el_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["getEL"],$pid);
}
function backend_get_start_rrdm_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["startRRDM"],$pid);
}
function backend_get_inspect_rrdm_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["getRRDM"],$pid);	
}
function backend_get_jobs_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["getJob"],$pid);	
}

function backend_get_charmap_route($pid) {
	global $config;
	return sprintf($config["backend"]["internalURL"] .
		$config["backend"]["routes"]["getCharmap"],$pid);	
}
?>
