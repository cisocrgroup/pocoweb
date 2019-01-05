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
        case 'languages' : get_languages();break;
        case 'get_projects' : get_projects();break;
        case 'get_project' : get_project();break;
        case 'delete_project' : delete_project();break;
        case 'correct_line' : correct_line();break;

        case 'get_page' : get_page();break;
        case 'create_project' : create_project();break;
        case 'get_users' : get_users();break;
        case 'get_user' : get_user();break;
        case 'update_user' : update_user();break;
        case 'delete_user' : delete_user();break;
        case 'create_user' : create_user();break;

    }
}

function get_languages(){

  $api = new Api(backend_get_languages_route("local"));
  $api->set_session_id(backend_get_session_cookie());
  $api->get_request();
  $status = $api->get_http_status_code();

  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        header("status: ".$status);
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


function get_project(){
}

function delete_project(){

  $pid = $_POST['pid'];
  $api = new Api(backend_get_remove_project_route($pid));
  $api->set_session_id(backend_get_session_cookie());
  $api->delete_request();

 $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        print_r($session); 
        print_r($status); 
        //echo json_encode("Successfully deleted project '"+$pid+"'"); 
    break;
  case "403":
    header("status: ".$status);
    echo  backend_get_http_status_info($status);
    break;
  default:
        header("status: ".$status);
        echo backend_get_http_status_info($status);
    break;
  }

}

function get_page(){

  $pid = $_POST['pid'];
  $p = $_POST['page'];
  
  $api_result;

  if ($p === "first") {
    $api = new Api(backend_get_first_page_route($pid));
    $api->set_session_id(backend_get_session_cookie());
    $api->get_request();
    $api_result = $api;

  } else if ($p == "last") {
    $api = new Api(backend_get_last_page_route($pid));
    $api->set_session_id(backend_get_session_cookie());
    $api->get_request();
    $api_result = $api;

  } else {
    $api = new Api(backend_get_nth_page_route($pid, $p));
    $api->set_session_id(backend_get_session_cookie());
    $api->get_request();
    $api_result = $api;

  }


  $status = $api_result->get_http_status_code();

  if ($status != 200) {
     header("status: ".$status);
    echo("error: Could not load project #$pid, page #$p: " .backend_get_http_status_info($status));

  } else {
    $page = $api_result->get_response();
    echo json_encode($page);




    // echo '<div id="page-view" onload=\'pcw.setErrorsDropdown(', $pid, ')\'>', "\n";
    // frontend_render_page_header_div($page);
    // frontend_render_page_heading_div($page);
    // frontend_render_page_div($page);
    // frontend_render_page_correct_all_div($page);
    // echo '</div>', "\n";
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

function correct_line() {

  $data = array(
    'correction' => $_POST['text'],
  );

  $api = new Api(backend_get_correct_line_route($_POST['pid'],$_POST['page_id'],$_POST['line_id']));
  $api->set_session_id(backend_get_session_cookie());
  $api->post_request($data);
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
 

}

function update_user() {


if (!isset($_POST["name"]) || !isset($_POST["email"]) ||
        !isset($_POST["institute"]) || !isset($_POST["password"]) ||
        !isset($_POST["new_password"])) {
        header("status: 500");
         echo "Invalid post parameters.";
    } else if ($_POST["password"] != $_POST["new_password"]) {
       header("status: 500");
        echo "Passwords do not match.";
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
function create_user() {

    if ($_POST["password"] != $_POST["new_password"]) {
       header("status: 500");
       echo "Error: could not create user '". $_POST['name']."' passwords do not match";
    } else {

     $admin = isset($_POST['admin']) && $_POST['admin'] == "on";

   $data = array(
        'user' => array (
            'name' => $_POST['name'],
            'email' => $_POST['email'],
            'institute' => $_POST['institute'],
            'admin' =>  $admin
        ),
    );
    if ($_POST['password'] != "") {
        $data['password'] = $_POST['password'];
    }

  $api = new Api(backend_get_users_route());
    $api->set_session_id(backend_get_session_cookie());
    $api->post_request($data);

  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();

        echo json_encode("Successfully created new user."); 


    break;
  case "403":
    header("status: ".$status);
    echo  backend_get_http_status_info($status).'. Error: Could not create user.';
    break;
  default:
        header("status: ".$status);
    echo  backend_get_http_status_info($status).'. Error: Could not create user.';
    break;
  }

  }
}

  




function delete_user(){

  $api = new Api(backend_get_delete_user_route($_POST['id']));
  $api->set_session_id(backend_get_session_cookie());
  $api->delete_request();
 
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();

        echo json_encode($session); 

    break;
  case "403":
    header("status: ".$status);
    echo  backend_get_http_status_info($status);
    break;
  default:
        header("status: ".$status);
        echo backend_get_http_status_info($status);
    break;
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