<?php
$config = array(
	"backend" => array(
		"url"    => "http://pocoweb.cis.lmu.de/rest",
		"routes" => array(
			"api_version"    => "/api-version",
			"login"          => "/login",
			"get_projects"   => "/books",
			"get_first_page" => "/books/%d/pages/first",
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
<!--
vim: tw=4
-->
