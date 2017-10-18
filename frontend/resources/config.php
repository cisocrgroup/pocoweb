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
		"url"            => "http://pocoweb.cis.lmu.de/rest",
		"profilerWaitMs" => 30000,
		"routes"         => array(
			"apiVersion"          => "/api-version",
			"login"               => "/login",
			"logout"              => "/logout",
			"get_projects"        => "/books",
			"splitProject"        => "/books/%d/split",
			"assignProject"       => "/books/%d/assign",
			"finishProject"       => "/books/%d/finish",
			"removeProject"       => "/books/%d",
			"downloadProject"     => "/books/%d/download",
			"getUsers"            => "/users",
			"deleteUser"          => "/users/%d",
			"getFirstPage"        => "/books/%d/pages/first",
			"getLastPage"         => "/books/%d/pages/last",
			"getNthPage"          => "/books/%d/pages/%d",
			"correctLine"         => "/books/%d/pages/%d/lines/%d",
			"correctWord"         => "/books/%d/pages/%d/lines/%d/words/%d",
			"uploadProject"       => "/books",
			"search"              => "/books/%d/search?q=%s&p=%d",
			"splitImages"         => "/books/%d/pages/%d/lines/%d/tokens/%d/split-images",
			"orderProfile"        => "/books/%d/profile",
			"searchSuggestions"   => "/books/%d/suggestions?q=%s",
			"getAllSuggestions"   => "/books/%d/suggestions",
			"getSuggestions"      => "/books/%d/pages/%d/lines/%d/tokens/%d/suggestions",
			"getSuspiciousWords"  => "/books/%d/pages/%d/lines/%d/suspicious-words",
			"languages"           => "/profiler-languages?url=%s",
			"adaptiveTokens"      => "/books/%d/adaptive-tokens",
			"updateUser"          => "/users/%d",
		),
	),
	"cookies" => array(
		"name"    => "pcw-user",
		"sid"     => "pcw-sid",
		"expires" => 8 * 60 * 60, // expires after 8 hours
		"domain"  => "pocoweb.cis.lmu.de",
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

defined("LIBRARY_PATH") or define("LIBRARY_PATH", realpath(dirname(__FILE__)) . "/library");

defined("TEMPLATES_PATH") or define("TEMPLATES_PATH", realpath(dirname(__FILE__)) . "/templates");

if ($config["frontend"]["debug"]) {
	ini_set("error_reporting", true);
	ini_set("display_errors", "On");
}
error_reporting(E_ALL|E_STRCT);
?>
