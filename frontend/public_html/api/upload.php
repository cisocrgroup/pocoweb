<?php
// $ds          = DIRECTORY_SEPARATOR;  //1
 
// $storeFolder = '../upload';   //2
require_once('config.php');
require_once('api.php');
require_once('backend.php');

if(count($_FILES)>0) print_r($_FILES);

print_r($_POST);

$file = $_FILES['archive'];
$post = $_POST;

$post["file"] = $file["name"];
	$api = new Api(backend_get_upload_project_route($file["name"]));
	$api->set_session_id(backend_get_session_cookie());
	$api->post_request($post);

	$status = $api->get_http_status_code();
	if ($status != 201 || $status != 200) { # accept 200 OK and 201 Created
		//frontend_render_error_div("Could not upload archive: backend returned " .
		print_r(backend_get_http_status_info($status));
	} else {
		//frontend_render_success_div("Successfully uploaded new project");
	}


// if (!empty($_FILES)) {
     
//     $tempFile = $_FILES['file']['tmp_name'];          //3             
      
//     $targetPath = dirname( __FILE__ ) . $ds. $storeFolder . $ds;  //4
     
//     $targetFile =  $targetPath. $_FILES['file']['name'];  //5
 
//     move_uploaded_file($tempFile,$targetFile); //6
     
// }


?>     
