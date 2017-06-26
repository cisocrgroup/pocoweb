<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <link
	rel="stylesheet"
	href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css"
	integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u"
	crossorigin="anonymous"/>
    <link rel="stylesheet" type="text/css" href="css/main.css"/>
    <title>PoCoWeb - Post Correction Web</title>
</head>

<body>
<div class="container-fluid">
    <div class="row">
	<img class="image-rounded center-block" src="img/lmu_cis_logo.jpg"/>
    </div>

    <!-- <div class="row"> -->
	<div id="navbar" class="navbar-collapse collapse">
	    <!-- <nav class="navbar navbar-default"> -->
		<!-- <div class="navbar-header"> -->
		    <ul class="nav navbar-nav navbar-left">
			<li><a href="index.php">Home</a></li>
			<li><a href="about.php">About</a></li>
			<li><a href="documentation.php">Documentation</a></li>
			<li><a href="http://www.cis.lmu.de">CIS</a></li>
		    </ul>
		    <ul class="nav navbar-nav navbar-right">
		    <?php
			require_once(dirname(dirname(__FILE__)) . "/config.php");
			require_once(LIBRARY_PATH . "/backend.php");
			if (backend_is_logged_in()) {
			    echo('<li><p class="navbar-text">Logged in as '
				. backend_get_login_name() . '</p></li>');
			} else {
			    // if not logged in, index.php with no parameters
			    // will render the login page.
			    echo('<li><a href="login.php">Login</a></li>');
			}
			$version = backend_get_api_version();
			if ($version === "unknown") {
			    $version = '<span class="glyphicon glyphicon-warning-sign"/>';
			}
			echo'<li><p class="navbar-text">Api-Version: ', $version, '</p></li>';
		    ?>
		    </ul>
		<!-- </div> -->
	    <!-- </nav> -->
	</div>
    <!-- </div> -->
    <!--
    <h1>Simple Site</h1>
    <ul class="nav global">
	<li><a href="#">Home</a></li>
	<li><a href="#">Articles</a></li>
	<li><a href="#">Portfolio</a></li>
	<li><a href="#">Login</a></li>
    </ul>
    -->
<!--
vim: sw=4
-->
