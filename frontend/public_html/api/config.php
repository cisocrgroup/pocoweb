<?php
# global user name and session id (if logged in)
global $USER;
$USER = NULL;
global $SID;
$SID = "";

# global configuration
global $config;
$config = array(
	"frontend" => array(
		"debug" => true,
		"upload" => array(
			"max_size"   => 2 * 1024 * 1024 * 1024, // 2gb
			"upload_dir" => "/tmp",
		),
		"image" => array(
			"line_image_height" => 25,
		),
	),
	"backend" => array(
	    "internalURL"    => "/rest",#http://pcwauth",
		"externalURL"    => "/rest", #"http://pocoweb.cis.lmu.de/rest",
		"profilerWaitMs" => 30000,
		"routes"         => array(
			"apiVersion"          => "/api-version",
			"login"               => "/login",
			"logout"              => "/logout",
			"get_projects"        => "/books",
			"getProject"          => "/books/%d",
			"splitProject"        => "/books/%d/split",
			"assignProject"       => "/books/%d/assign?uid=%d",
			"finishProject"       => "/books/%d/finish",
			"removeProject"       => "/books/%d",
			"downloadProject"     => "/books/%d/download",
			"getUsers"            => "/users",
			"deleteUser"          => "/users/%d",
			"getFirstPage"        => "/books/%d/pages/first",
			"getLastPage"         => "/books/%d/pages/last",
			"getNthPage"          => "/books/%d/pages/%d",
			"correctLine"         => "/books/%d/pages/%d/lines/%d",
			"correctWord"         => "/books/%d/pages/%d/lines/%d/tokens/%d",
			"uploadProject"       => "/books",
			"search"              => "/books/%d/search?q=%s&p=%d",
			"splitImages"         => "/books/%d/pages/%d/lines/%d/tokens/%d/split-images",
			"orderProfile"        => "/profile/books/%d",
			"searchSuggestions"   => "/profile/books/%d?q=%s",
			"getAllSuggestions"   => "/profile/books/%d",
			"getSuggestions"      => "/profile/books/%d?q=%s",
			"getSuspiciousWords"  => "/profile/suspicious/books/%d",
     		"getErrorPatterns"    => "/profile/patterns/books/%d?ocr=1",
			"languages"           => "/profile/languages?url=%s",
			"adaptiveTokens"      => "/profile/adaptive/books/%d",
			"updateUser"          => "/users/%d",
			"getGlobalOCRModels"  => "/ocr", #get
			"getOCRModels"        => "/ocr/books/%d", #get
			"runOCR"              => "/ocr/books/%d?train=%d", #post
			"startEL"             => "/postcorrect/el/books/%d", #post
			"getEL"               => "/postcorrect/el/books/%d", #get
			"startRRDM"           => "/postcorrect/rrdm/books/%d", #post
			"getRRDM"             => "/postcorrect/rrdm/books/%d", #get
			"getJob"              => "/jobs/%d", #get
		),
	),
	"cookies" => array(
		"name"    => "pcw-user",
		"sid"     => "pcw-sid",
		"expires" => 8 * 60 * 60, // expires after 8 hours
	),
	"httpStatusInfo" => array(
		100 => "Continue",
		101 => "Switching protocols",
		200 => "OK",
		201 => "Created",
		202 => "Accepted",
		203 => "Non-Authoritative Information",
		204 => "No Content",
		205 => "Reset Content",
		206 => "Partial Content",
		300 => "Multiple Choices",
		301 => "Moved Permanently",
		302 => "Found",
		303 => "See Other",
		304 => "Not Modified",
		305 => "Use Proxy",
		306 => "(Unused)",
		307 => "Temporary Redirect",
		400 => "Bad Request",
		401 => "Unauthorized",
		402 => "Payment Required",
		403 => "Forbidden",
		404 => "Not Found",
		405 => "Method Not Allowed",
		406 => "Not Acceptable",
		407 => "Proxy Authentication Required",
		408 => "Request Timeout",
		409 => "Conflict",
		410 => "Gone",
		411 => "Length Required",
		412 => "Precondition Failed",
		413 => "Request Entity Too Large",
		414 => "Request-URI Too Long",
		415 => "Unsupported Media Type",
		416 => "Requested Range Not Satisfiable",
		417 => "Expectation Failed",
		500 => "Internal Server Error",
		501 => "Not Implemented",
		502 => "Bad Gateway",
		503 => "Service Unavailable",
		504 => "Gateway Timeout",
		505 => "HTTP Version Not Supported",
	),
);

global $uploadErrorCodes;
$uploadErrorCodes = array(
	  UPLOAD_ERR_OK         => "OK",
		UPLOAD_ERR_INI_SIZE   => "PHP size limit exeeded",
		UPLOAD_ERR_FORM_SIZE  => "HTML size limit exeeded",
		UPLOAD_ERR_PARTIAL    => "partial upload",
		UPLOAD_ERR_NO_FILE    => "no file uploaded",
		UPLOAD_ERR_NO_TMP_DIR => "missing tmp directory",
		UPLOAD_ERR_CANT_WRITE => "could not write file",
		UPLOAD_ERR_EXTENSION  => "file upload stopped",
);

defined("LIBRARY_PATH") or define("LIBRARY_PATH", realpath(dirname(__FILE__)) . "/library");

defined("TEMPLATES_PATH") or define("TEMPLATES_PATH", realpath(dirname(__FILE__)) . "/templates");

if ($config["frontend"]["debug"]) {
	ini_set("error_reporting", true);
	ini_set("display_errors", "On");
}
error_reporting(E_ALL|E_STRCT);
?>
