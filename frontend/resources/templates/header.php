<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<!-- vim: set ts=4 sw=4 :-->
<html lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <link
	rel="stylesheet"
	href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css"
	integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u"
	crossorigin="anonymous"/>
    <link rel="stylesheet" type="text/css" href="css/pcw.css"/>
    <script src="//code.jquery.com/jquery-1.11.0.min.js"></script>
    <script src="//netdna.bootstrapcdn.com/bootstrap/3.1.1/js/bootstrap.min.js"></script>
    <script type="text/javascript" src="js/pcw.js"></script>
	<script type="text/javascript" src="js/config.js"></script>
	<script type="text/javascript" src="https://unpkg.com/xregexp@3.2.0/xregexp-all.js"></script>
    <link href="//netdna.bootstrapcdn.com/bootstrap/3.1.1/css/bootstrap.min.css" rel="stylesheet">
    <title>PoCoWeb - Post Correction Web</title>
</head>

<body>
<div class="container-fluid">
    <div id="pocweb-header">
    <div class="row">
	<img class="image-rounded center-block" src="img/lmu_cis_logo.jpg"/>
    </div>
	<div id="navbar" class="navbar-collapse collapse">
	    <!-- <nav class="navbar navbar-default"> -->
		<!-- <div class="navbar-header"> -->
		    <ul class="nav navbar-nav navbar-left">
			<li><a href="index.php">Home</a></li>
			<li><a href="users.php">Users</a></li>
			<li><a href="about.php">About</a></li>
			<li><a href="documentation.php">Documentation</a></li>
			<?php
			global $config;
			if ($config["frontend"]["debug"]) {
				echo('<li><a href="info.php">Info</a></li>');
			}
			?>
		    </ul>
		    <ul class="nav navbar-nav navbar-right">
<?php
require_once(dirname(dirname(__FILE__)) . "/config.php");
require_once(LIBRARY_PATH . "/backend.php");
backend_setup_globals();
global $USER;
if ($USER !== NULL) {
	echo '<li><p class="navbar-text">Logged in as user: ',
		$USER["name"], '</p></li>', "\n";
	echo '<li><a href="logout.php">Logout</a></li>', "\n";
} else {
    echo('<li><a href="login.php">Login</a></li>');
}
$version = '<span class="glyphicon glyphicon-warning-sign"/>';
global $API;
if ($API != "") {
	$version = $API;
}
echo '<li><p class="navbar-text">Api-Version: ', $version, '</p></li>';
?>
		    </ul>
    </div>
</div>
<?php
/*
require_once(LIBRARY_PATH . "/frontend.php");
global $config;
global $USER;
global $API;
global $SID;
if ($config["frontend"]["debug"]) {
	frontend_render_info_div("GLOBAL API:  $API");
	frontend_render_info_div("GLOBAL SID:  $SID");
	if ($USER != NULL) {
		frontend_render_info_div("GLOBAL USER: $USER[name] &lt;$USER[email]&gt;");
	} else {
		frontend_render_info_div("GLOBAL USER: NULL");
	}
}
 */
?>
