<?php
# global api version, user name and session id (if logged in)
global $USER;
$USER = NULL;
global $SID;
$SID = "";
global $API;
$API = "";

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
		"url"    => "http://pocoweb.cis.lmu.de/rest",
		"routes" => array(
			"api_version"      => "/api-version",
			"login"            => "/login",
			"logout"           => "/logout",
			"get_projects"     => "/books",
			"split_project"    => "/books/%d/split",
			"assign_project"   => "/books/%d/assign",
			"finish_project"   => "/books/%d/finish",
			"remove_project"   => "/books/%d",
			"download_project" => "/books/%d/download",
			"get_users"        => "/users",
			"delete_user"      => "/users/%d",
			"get_first_page"   => "/books/%d/pages/first",
			"get_last_page"    => "/books/%d/pages/last",
			"get_nth_page"     => "/books/%d/pages/%d",
			"correct_line"     => "/correction",
			"upload_project"   => "/books",
			"search"           => "/books/%d/search?q=%s",
			"split_images"     => "/books/%d/pages/%d/lines/%d/create-split-images",
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
