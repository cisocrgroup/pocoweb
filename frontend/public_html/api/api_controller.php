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


ini_set('max_execution_time', 300);
ini_set('max_input_vars ', 1000000);

if(isset($_POST['backend_route']) && !empty($_POST['backend_route'])) {
    $action = $_POST['backend_route'];
    switch($action) {
        case 'login' : login();break;
        case 'login_check' : login_check();break;
        case 'logout' : logout();break;
        case 'api_version' : get_api_version();break;
        case 'get_projects' : get_projects();break;
        case 'get_users' : get_users();break;
        case 'get_user' : get_user();break;
        case 'update_user' : update_user();break;

    }
}


function get_projects(){
  //print_r(backend_get_session_cookie());

  $api = new Api(backend_get_projects_route());
  $api->set_session_id(backend_get_session_cookie());
  $api->get_request();
 
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();

        echo json_encode($session); 

    break;
  case "403":
    header("status: ".$status);
    echo  backend_get_http_status_info($status).'. <a href="#" class="js-login">Please login.</a>';
    break;
  default:
        header("status: ".$status);
        echo backend_get_http_status_info($status);
    break;
  }

  return $api;
}

function get_users(){

  $api = new Api(backend_get_users_route());
  $api->set_session_id(backend_get_session_cookie());
  $api->get_request();
 
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();

        echo json_encode($session); 

    break;
  case "403":
    header("status: ".$status);
    echo  backend_get_http_status_info($status).'. <a href="#" class="js-login">Please login.</a>';
    break;
  default:
        header("status: ".$status);
        echo backend_get_http_status_info($status);
    break;
  }

  return $api;
}

function get_user(){

      if(!isset($_POST['id'])){
      if(isset(backend_get_session_cookie()['user'])){
        $session = backend_get_session_cookie();
        echo json_encode($session['user']);
      }
      else {
       header("status: 403");
        echo  backend_get_http_status_info("403").'. <a href="#" class="js-login">Please login.</a>';
      }

    }
    else{
      echo "to do";
    }
 
  // $status = $api->get_http_status_code();
  // switch ($status) {
  // case "200":
  //       $result=array();
  //       $session = $api->get_response();
  //       backend_get_session_cookie();
  //       global $USER;
  //       global $SID;
  //       $USER = $session["user"];
  //       $SID = $session["auth"];
  //   // backend_set_global_session_id();
  //   // backend_set_global_user();
  //       header_remove();
  //       header("status: ".$status);

  //       $result['user'] = $USER;
  //       $result['message'] = "You have successfully logged in";
  //       echo json_encode($result); 

  //   break;
  // case "403":
  //   $SID = "";
  //   header_remove();
  //   header("status: ".$status);
  //   echo 'Login error: invalid username or password. Please try again: <a href="#" class="js-login">login</a>';
  //   break;
  // default:
  //   $SID = "";
  //       header_remove();
  //       header("status: ".$status);
  //       echo "Login error: " . backend_get_http_status_info($status);
  //   break;
  // }

  // return $api;
}

function update_user() {

  

if (!isset($_POST["name"]) || !isset($_POST["email"]) ||
        !isset($_POST["institute"]) || !isset($_POST["password"]) ||
        !isset($_POST["new_password"])) {
        header("status: 500");
         echo "invalid post parameters";
    } else if ($_POST["password"] != $_POST["new_password"]) {
       header("status: 500");
        echo "passwords do not match";
    } else {

  $uid = (int)$_POST['id'];
    $data = array(
        'user' => array (
            'id' => $uid,
            'name' => $_POST['name'],
            'email' => $_POST['email'],
            'institute' => $_POST['institute'],
        ),
    );
    if ($_POST['password'] != "") {
        $data['password'] = $_POST['password'];
    }

  $api = new Api(backend_get_update_user_route($uid));
    $api->set_session_id(backend_get_session_cookie());
    $api->put_request($data);

  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
 
        /////////////////////
        $data = array("email" => $_POST['email'], "password" => $_POST['password']);
        $api = new Api(backend_get_login_route());
        $api->post_request($data);
        $session2 = $api->get_response();
        $session2['user']['password'] = $_POST['password']; // save pw in cookie = workaround...
        backend_set_session_cookie($session2);
        ////////////////////////

        echo json_encode($session); 


    break;
  case "403":
    header("status: ".$status);
    echo  backend_get_http_status_info($status).'. <a href="#" class="js-login">Please login.</a>';
    break;
  default:
        header("status: ".$status);
        echo backend_get_http_status_info($status);
    break;
  }
}
}

function login(){

  $data = array("email" => $_POST['email'], "password" => $_POST['password']);
  $api = new Api(backend_get_login_route());
  $api->post_request($data);

  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();

        $session['user']['password'] = $_POST['password']; // save pw in cookie = workaround...
        backend_set_session_cookie($session);



        header("status: ".$status);

        $result['user'] = $session['user'];
        $result['message'] = "You have successfully logged in";
        echo json_encode($result); 

    break;
  case "403":
    header("status: ".$status);
    echo 'Login error: invalid username or password. Please try again: <a href="#" class="js-login">login</a>';
    break;
  default:
        header("status: ".$status);
        echo "Login error: " . backend_get_http_status_info($status);
    break;
  }

}

function logout(){

  $api = new Api(backend_get_logout_route());
  $api->set_session_id(backend_get_session_cookie());
  $api->get_request();
 
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
       // backend_set_session_cookie("");
        unset($_COOKIE['pcw-sid']);
        setcookie('pcw-sid', '', time() - 3600, '/');

        header("status: ".$status);

        $result['message'] = 'You have successfully logged out! <a href="#" class="js-login">Log back in?</a>';
        echo json_encode($result); 

    break;
  case "403":
         //   backend_set_session_cookie("");
            unset($_COOKIE['pcw-sid']);
            setcookie('pcw-sid', '', time() - 3600, '/');


    header("status: ".$status);
    echo  backend_get_http_status_info($status);
    break;
  default:
        header("status: ".$status);
        echo backend_get_http_status_info($status);
    break;
  }

  return $api;
}

function login_check(){

      if(isset(backend_get_session_cookie()['user'])){
        $session = backend_get_session_cookie();
        echo json_encode($session['user']);

      }
      else {
        echo -1;
      }
 
}


function get_api_version(){
  $api = new Api(backend_get_api_version_route());
  $api->get_request();

  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result = $api->get_response();
  
        header_remove();
        header("status: ".$status);

        echo json_encode($result); 

    break;
  default:
        header_remove();
        header("status: ".$status);
        echo 'Couldn\'t get api version';
    break;
  }


  return $api;
}

?>