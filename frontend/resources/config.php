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
			"correctLine"         => "/books/%d/pages/%d/lines/%d/correct",
			"correctWord"         => "/books/%d/pages/%d/lines/%d/words/%d/correct",
			"uploadProject"       => "/books",
			"search"              => "/books/%d/search?q=%s&p=%d",
			"splitImages"         => "/books/%d/pages/%d/lines/%d/tokens/%d/split-images",
			"orderProfile"        => "/books/%d/profile",
			"searchSuggestions"   => "/books/%d/suggestions?q=%s",
			"getAllSuggestions"   => "/books/%d/suggestions",
			"getSuggestions"      => "/books/%d/pages/%d/lines/%d/tokens/%d/suggestions",
			"languages"           => "/profiler-languages",
		),
	),
	"cookies" => array(
		"name"    => "pcw-user",
		"sid"     => "pcw-sid",
		"expires" => 8 * 60 * 60, // expires after 8 hours
		"domain"  => "pocoweb.cis.lmu.de",
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
