<?php
require_once('config.php');
require_once('api.php');
require_once('backend.php');

$file = $_FILES['archive'];
$post = $_POST;

    error_log("uploading file size=$file[size] type=$file[type] tmp_name=$file[tmp_name]");
	if ($file["error"] != UPLOAD_ERR_OK) {
        $info = backend_get_upload_error_info($file["error"]);
		echo("Could not upload archive: error: ".$info);
		return;
	}
	if ($file["size"] > $config["frontend"]["upload"]["max_size"]) {
		echo ("Could not upload archive: file too big");
		return;
	}
	if ($file["type"] != "application/zip") {
		echo ("Could not upload archive: not a zip file");
		return;
	}
	if (!file_exists($file["tmp_name"])) {
		echo ("Could not upload archive: upload file does not exist");
        return;
	}
	if (!chmod($file["tmp_name"], 0755)) {
		echo ("Could not upload archive: could publish upload file");
        return;
	}

$post["file"] = $file["tmp_name"];
	$api = new Api(backend_get_upload_project_route($file["name"]));
	$api->set_session_id(backend_get_session_cookie());
	$api->post_request($post);

	$status = $api->get_http_status_code();
	if ($status == 201 || $status == 200) { # accept 200 OK and 201 Created
			  echo ("Successfully uploaded new project");
	} else {
		print_r(backend_get_http_status_info($status));

	}

?>
