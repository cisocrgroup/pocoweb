<?php

// header('Access-Control-Allow-Origin: *');
// header('Access-Control-Allow-Methods',
// 'GET,POST,PUT,DELETE,OPTIONS');

#include 'connect.php';
#include 'dbutil.php';
#include 'query.php';
require_once('config.php');
require_once('api.php');
require_once('backend.php');

ob_start();
session_start();

ini_set('max_execution_time', 300);
ini_set('max_input_vars ', 1000000);

if(isset($_POST['backend_route']) && !empty($_POST['backend_route'])) {
    $action = $_POST['backend_route'];
    switch($action) {
        case 'login' : login();break;
    }
}


function login(){

  $data = array("email" => $_POST['email'], "password" => $_POST['password']);
  $api = new Api(backend_get_login_route());
  $api->post_request($data);

  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $session = $api->get_response();
        backend_set_session_cookie($session);
        global $USER;
        global $SID;
        $USER = $session["user"];
        $SID = $session["auth"];
    // backend_set_global_session_id();
    // backend_set_global_user();
        header_remove();
        header($status);
        echo "You have successfully logged in";

    break;
  case "403":
    $SID = "";
    require(TEMPLATES_PATH . "/header.php");
    frontend_render_warning_div(
      "Login error: invalid username or password");
    frontend_render_login_div();
    break;
  default:
    $SID = "";
    require(TEMPLATES_PATH . "/header.php");
    frontend_render_error_div("Login error: " . backend_get_http_status_info($status));
    frontend_render_login_div();
    break;
  }
// } else {
//   require(TEMPLATES_PATH . "/header.php");
//   frontend_render_login_div("Internal error: bad request to login.php");
// }

  return $api;
}

?>