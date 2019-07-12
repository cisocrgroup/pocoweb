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
        case 'update_project' : update_project();break;
        case 'split_project' : split_up_project();break;
        case 'download_project' : download_project();break;
        case 'delete_project' : delete_project();break;
        case 'assign_package' : assign_package();break;
        case 'assign_packages' : assign_packages();break;
        case 'get_line' : get_line();break;
        case 'correct_line' : correct_line();break;
        case 'correct_token' : correct_token();break;
        case 'search_token' : search_token();break;
        case 'get_error_patterns' : get_error_patterns();break;
        case 'get_correction_suggestions' : get_correction_suggestions();break;
        case 'get_all_correction_suggestions' : get_all_correction_suggestions();break;
        case 'get_suspicious_words' : get_suspicious_words();break;
        case 'get_split_images' : get_split_images();break;
        case 'order_profile' : order_profile();break;
        case 'documentation' : get_documentation();break;
        case 'get_page' : get_page();break;
        case 'create_project' : create_project();break;
        case 'get_users' : get_users();break;
        case 'get_user' : get_user();break;
        case 'update_user' : update_user();break;
        case 'delete_user' : delete_user();break;
        case 'create_user' : create_user();break;
        case 'start_lexicon_extension' : start_lexicon_extension();break;
        case 'inspect_extended_lexicon' : inspect_extended_lexicon();break;
        case 'profile_le' : profile_le();break;
        case 'start_postcorrection' : start_postcorrection();break;
        case 'inspect_postcorrection' : inspect_postcorrection();break;
        case 'get_jobs' : get_jobs();break;
        case 'get_charmap': get_charmap(); break;
        default: internal_server_error(); break;
    }
}

function internal_server_error() {
    header("status: 500");
    echo backend_get_http_status_info("500");
}

function get_split_images(){

    $word = $_POST['word'];
    $api = new Api(backend_get_split_images_route(
    $word["projectId"], $word["pageId"],
    $word["lineId"], $word["tokenId"]));
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

function get_documentation(){



 $text = file_get_contents("../doc.md");

 // print_r($text);


  $data = array(
    'text' => $text,
    'mode' => "markdown"
  );

 $data_string = json_encode($data);

  $url = "https://api.github.com/markdown";
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_USERAGENT, "pocoweb");
  curl_setopt($ch, CURLOPT_CAINFO, dirname(__FILE__)."/cacert.pem");
  curl_setopt($ch, CURLOPT_URL,$url);
  curl_setopt($ch, CURLOPT_POST,1);
  curl_setopt($ch, CURLOPT_POSTFIELDS,$data_string);
  curl_setopt($ch, CURLOPT_HTTPHEADER, array(
      'Content-Type: application/json',
      'Content-Length: ' . strlen($data_string))
  );

  curl_setopt($ch, CURLOPT_TIMEOUT, 30);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
  $status_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);

  $htmlstring=curl_exec ($ch);

  echo $htmlstring;

;

}


function get_project(){

  $pid = $_POST['pid'];
  $api = new Api(backend_get_project_route($pid));
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
    echo  backend_get_http_status_info($status);
    break;
  default:
        header("status: ".$status);
        echo backend_get_http_status_info($status);
    break;
  }
}

function delete_project(){

  $pid = $_POST['pid'];
  $api = new Api(backend_get_project_route($pid));
  $api->set_session_id(backend_get_session_cookie());
  $api->delete_request();

 $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        echo json_encode("Successfully deleted project '"+$pid+"'");
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

function update_project(){

  $pid = $_POST['pid'];
  $api = new Api(backend_get_project_route($pid));
  $api->set_session_id(backend_get_session_cookie());
  $api->post_request($_POST['projectdata']);

  # force numeric year
  $year = intval($_POST['projectdata']['year']);
  $_POST['projectdata']['year'] = $year;
  # handle `\uxxxx` in hist pattern input
  $histPatterns = $_POST['projectdata']['histPatterns'];
  $histPatterns = json_decode("\"$histPatterns\"");
  $_POST['projectdata']['histPatterns'] = $histPatterns;

  $api->post_request($_POST['projectdata']);

 $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        echo json_encode("Successfully deleted project '"+$pid+"'");
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

function split_up_project(){

  $pid = $_POST['pid'];
  $api = new Api(backend_get_split_project_route($pid));
  $api->set_session_id(backend_get_session_cookie());


  $data = array("n" => $_POST['n']);

  if ($_POST['random'] && $_POST["random"] == true) {
    $data["random"] = true;
  } else {
    $data["random"] = false;
  }


  $api->post_request($data);

 $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        echo json_encode($session);
        // echo ('Project '.$pid." successfully splitted");
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

function assign_packages(){
  $pairs = $_POST['pairs'];

  // print_r($pairs);
  $count = 0;
  for($i=0;$i<sizeof($pairs);$i++){

      // echo $pairs[$i]['pid']." ".$pairs[$i]['uid'];

      $api = new Api(backend_get_assign_project_route($pairs[$i]['pid'],$pairs[$i]['uid']));
      $api->set_session_id(backend_get_session_cookie());

      $api->get_request();

     $status = $api->get_http_status_code();
      switch ($status) {
      case "200":
            $result=array();
            $session = $api->get_response();
            // echo ('Successfully assigned project '.$pairs[$i]['pid']." to user ".$pairs[$i]['uid']);
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
    $count ++;
  }

  echo ('Successfully assigned '.$count.' projects.');

}

function assign_package(){
  $pid = $_POST['pid'];
  $pid = $_POST['uid'];
  $api = new Api(backend_get_assign_project_route($pid,$uid));
  $api->set_session_id(backend_get_session_cookie());

  $api->get_request();

 $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        echo ('Successfully assigned project '.$pid." to user "+$uid);
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




function download_project(){

  $pid = $_POST['pid'];
  $api = new Api(backend_get_download_project_route($pid));
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
    echo  backend_get_http_status_info($status);
    break;
  default:
        header("status: ".$status);
        echo backend_get_http_status_info($status);
    break;
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


function correct_line() {
    // Treat post data as json string to automatically convert \u017f etc
  $data = array(
      'correction' => json_decode("\"$_POST[text]\""),
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



function get_line() {

  $api = new Api(backend_get_correct_line_route($_POST['pid'],$_POST['page_id'],$_POST['line_id']));
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

function correct_token() {
    // Treat post data as json string to automatically convert \u017f etc
    $data = array(
        'correction' => json_decode("\"$_POST[token]\""),
    );

  $api = new Api(backend_get_correct_word_route($_POST['pid'],$_POST['page_id'],$_POST['line_id'],$_POST['token_id']));
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


function order_profile() {
  $api = new Api(backend_get_order_profile_route($_POST['pid']));
  $api->set_session_id(backend_get_session_cookie());
  $data = array("tokens" => array());
  $api->post_request($data);
  $status = $api->get_http_status_code();
  switch ($status) {
  case "202":
        $result=array();
        $session = $api->get_response();
        echo "Profile for project ".$_POST['pid']." successfully ordered.";
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

function search_token() {

  $api = new Api(backend_get_search_route($_POST['pid'],$_POST['q'],$_POST['isErrorPattern']));
  $api->set_session_id(backend_get_session_cookie());
  $api->get_request();
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        print_r(json_encode($session));
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

function get_error_patterns() {

  $api = new Api(backend_get_error_patterns_route($_POST['pid']));

  $api->set_session_id(backend_get_session_cookie());
  $api->get_request();
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        print_r(json_encode($session));
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

function get_correction_suggestions() {

  $api = new Api(backend_get_suggestions_route($_POST['pid'],$_POST['q']));
  $api->set_session_id(backend_get_session_cookie());
  $api->get_request();
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        print_r(json_encode($session));
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
function get_all_correction_suggestions() {


  $api = new Api(backend_get_all_suggestions_route($_POST['pid']));
  $api->set_session_id(backend_get_session_cookie());
  $api->get_request();
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        print_r(json_encode($session));
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

function get_suspicious_words() {


  $api = new Api(backend_get_suspicious_words_route($_POST['pid']));
  $api->set_session_id(backend_get_session_cookie());
  $api->get_request();
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        print_r(json_encode($session));
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

function start_lexicon_extension() {

  $data = array("1" => true);

  $api = new Api(backend_get_start_el_route($_POST['pid']));
  $api->set_session_id(backend_get_session_cookie());
  $api->post_request($data);

  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        print_r(json_encode($session));
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

function inspect_extended_lexicon() {


  $api = new Api(backend_get_inspect_el_route($_POST['pid']));
  $api->set_session_id(backend_get_session_cookie());
  $api->get_request();
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        print_r(json_encode($session));
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

function profile_le() {

  $data = array("projectId" => $_POST['pid'],"tokens" => $_POST['extensions'], "bookId" => $_POST['bid']);

  $api = new Api(backend_get_order_profile_route($_POST['pid']));
  $api->set_session_id(backend_get_session_cookie());
  $api->post_request($data);
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        print_r(json_encode($session));
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

function start_postcorrection() {

  $data = array("tokens" => []);

  $api = new Api(backend_get_start_rrdm_route($_POST['pid']));
  $api->set_session_id(backend_get_session_cookie());
  $api->post_request($data);
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        print_r(json_encode($session));
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
function inspect_postcorrection() {


  $api = new Api(backend_get_inspect_rrdm_route($_POST['pid']));
  $api->set_session_id(backend_get_session_cookie());
  $api->get_request();
  $status = $api->get_http_status_code();
  switch ($status) {
  case "200":
        $result=array();
        $session = $api->get_response();
        print_r(json_encode($session));
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

function get_jobs(){
  $api = new Api(backend_get_jobs_route($_POST['pid']));
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

function get_charmap(){
  $api = new Api(backend_get_charmap_route($_POST['pid']));
  $api->set_session_id(backend_get_session_cookie());
  $api->add_parameter("filter", $_POST['filter']);
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
    $auth = backend_get_session_cookie();
	$api = new Api(backend_get_login_route());
	$api->set_session_id($auth);
	$api->get_request();
    $status = $api->get_http_status_code();
    if ($status != "200") {
        header("status: " . $status);
        echo backend_get_http_status_info($status);
        return;
    }
    echo json_encode($api->get_response()['user']);
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

        $admin = isset($_POST['admin']) && ($_POST['admin'] == "on" || $_POST['admin'] == "true");

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

        $session['user']['password'] = $_POST['password']; // TODO: save pw in cookie = workaround...
        backend_set_session_cookie($session);



        header("status: ".$status);

        $result['user'] = $session['user'];
        $result['message'] = "You have successfully logged in!";
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
        $result=array();
        // unset($_COOKIE['pocoweb-auth']);
        setcookie('pocoweb-auth', '', time() + 20);

        header("status: ".$status);

        $result['message'] = 'You have successfully logged out! <a href="#" class="js-login">Log back in?</a>';
        echo json_encode($result);
}

function login_check(){
      if(backend_get_session_cookie()!=""){
        $api = new Api(backend_get_login_route());
        $api->set_session_id(backend_get_session_cookie());
        $api->get_request();
        $status = $api->get_http_status_code();
        if ($status != "200") {
            echo -1;
            return;
        }
        $result=array();
        header("status: ".$status);
        $session = $api->get_response();
        echo json_encode($session);
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

}

?>
