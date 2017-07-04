<?php
$config = array(
	"frontend" => array(
		"debug" => true,
	),
	"backend" => array(
		"url"    => "http://pocoweb.cis.lmu.de/rest",
		"routes" => array(
			"api_version"    => "/api-version",
			"login"          => "/login",
			"logout"         => "/logout",
			"get_projects"   => "/books",
			"split_project"  => "/books/%d/split",
			"assign_project" => "/books/%d/assign",
			"finish_project" => "/books/%d/finish",
			"get_users"      => "/users",
			"delete_user"    => "/users/%d",
			"get_first_page" => "/books/%d/pages/first",
			"get_last_page"  => "/books/%d/pages/last",
			"get_nth_page"   => "/books/%d/pages/%d",
			"correct_line"   => "/books/%d/pages/%d/lines/%d",
			"upload_project" => "/books",
		),
		"upload" => array(
			"max_size"   => 2 * 1024 * 1024 * 1024, // 2gb
			"upload_dir" => "/tmp",
		),
	),
	"cookies" => array(
		"name" => "pcw-user",
		"sid"  => "pcw-sid",
	),
);
defined("LIBRARY_PATH") or define("LIBRARY_PATH", realpath(dirname(__FILE__)) . "/library");

defined("TEMPLATES_PATH") or define("TEMPLATES_PATH", realpath(dirname(__FILE__)) . "/templates");

ini_set("error_reporting", true);
ini_set("display_errors", "On");
error_reporting(E_ALL|E_STRCT);

?>
