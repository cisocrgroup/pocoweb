<?php

require_once(dirname(dirname(__FILE__)) . "/config.php");
require_once(LIBRARY_PATH . "/backend.php");

function frontend_render_login_div() {
	echo('<div class="row">');
	// echo('<div class="input-group col-md-3 col-md-offset-2">');
	echo('<form action="login.php" method="post">');
	echo('<input name="name" type="text" class="form-control" placeholder="Username" />');
	echo('<input name="pass" type="password" class="form-control" placeholder="Password" />');
	echo('<input type="submit" value="Login"/>');
	echo('</form>');
	// echo('</div>');
	echo('</div>');
}

function frontend_render_projects_div() {
		frontend_render_home_div();
		frontend_render_upload_new_project_div();
		frontend_render_project_table_div();
}

function frontend_render_home_div() {
	echo('<div id="home">');
	// echo('<div class="col-md-5 col-md-offset-1">');
	echo('<h1>Home</h1>');
	// echo('</div>');
	echo('</div>');
}

function frontend_render_project_table_div() {
	$projects = backend_get_projects();
	if ($projects === NULL) {
		frontend_render_error_div("internal error: could not load projects");
	} else {
		// echo '<div class="row">', "\n";
		// echo '<div class="col-md-5 col-md-offset-1">', "\n";
		echo '<h2>Projects</h2>', "\n";
		frontend_render_project_table($projects);
		// echo '</div>', "\n";
		// echo '</div>', "\n";
	}
}

function frontend_render_project_table($projects) {
	echo '<table class="table table-striped">', "\n";
	frontend_render_project_table_header();
	foreach ($projects["books"] as $project) {
		frontend_render_project_table_row($project);
	}
	echo '</table>', "\n";
}

function frontend_render_project_table_row($project) {
	$pid = $project["projectId"];
	echo '<tr>';
	echo '<td>', $pid, '</td>';
	echo '<td>', frontend_get_project_table_value($project["author"]), '</td>';
	echo '<td>', frontend_get_project_table_value($project["title"]), '</td>';
	echo '<td>', frontend_get_project_table_value($project["year"]), '</td>';
	echo '<td>', frontend_get_project_table_value($project["language"]), '</td>';
	echo '<td>', frontend_get_project_table_value($project["pages"]), '</td>';
	echo '<td>', frontend_get_project_table_value($project["isBook"]), '</td>';
	echo '<td>';
	echo frontend_get_project_table_action_button(
		"open project", $pid, "glyphicon glyphicon-ok");
	echo frontend_get_project_table_action_button(
		"delete project", $pid, "glyphicon glyphicon-remove");
	echo frontend_get_project_table_action_button(
		"download project", $pid, "glyphicon glyphicon-download");
	echo '</tr>', "\n";
}

function frontend_render_project_table_header() {
	echo '<tr>';
	echo '<th>#</th>';
	echo '<th>Author</th>';
	echo '<th>Title</th>';
	echo '<th>Year</th>';
	echo '<th>Langauge</th>';
	echo '<th>Pages</th>';
	echo '<th>Book</th>';
	echo '<th>Actions<th/>';
	echo '</tr>', "\n";
}

function frontend_get_project_table_action_button($msg, $id, $class) {
	return '<button onclick="window.location.href=\'page.php?n=0&p=first&pid='
		. $id . '\'" title="' . $msg . ' #' . $id . '">'
		. '<span class="' . $class . '"/>'
		. '</button>';
}

function frontend_get_project_table_value($val) {
	if ($val === TRUE) {
		return '<span class="glyphicon glyphicon-ok-circle"/>';
	} else if ($val === FALSE) {
		return '<span class="glyphicon glyphicon-remove-circle"/>';
	} else if ($val === "") {
		return '<span class="glyphicon glyphicon-ban-circle"/>';
	} else {
		return $val;
	}
}

function frontend_render_upload_new_project_div() {
	echo '<div id="upload-project">', "\n";
	echo '<h2>Upload new project</h2>', "\n";
	echo '<form action="upload.php" method="post">', "\n";
	// Author
	echo '<div class="form-group">', "\n";
	echo '<label for="author">Project\'s author</label>', "\n";
	echo '<input name="author" type="text" placeholder="Author" class="form-control"/>', "\n";
	echo '</div>', "\n";
	// Title
	echo '<div class="form-group">', "\n";
	echo '<label for="title">Project\'s title</label>', "\n";
	echo '<input name="title" type="text" placeholder="Title" class="form-control"/>', "\n";
	echo '</div>', "\n";
	// Year
	echo '<div class="form-group">', "\n";
	echo '<label for="year">Year of publication</label>', "\n";
	echo '<input name="year" type="number" min="0" max="2099" step="1" value="2017" class="form-control"/>', "\n";
	echo '</div>', "\n";
	// Language
	echo '<div class="form-group">', "\n";
	echo '<label for="lang">Language</label>', "\n";
	echo '<input name="lang" type="text" placeholder="Language" class="form-control"/>', "\n";
	echo '</div>', "\n";
	// Description
	echo '<div class="form-group">', "\n";
	echo '<label for="desc">Description</label>', "\n";
	echo '<input name="desc" type="text" placeholder="Description" class="form-control"/>', "\n";
	echo '</div>', "\n";
	// upload file
	echo '<div class="input-group">', "\n";
	echo '<label class="input-group-btn">', "\n";
	echo '<span class="btn">', "\n";
	echo 'Browse&hellip; ', "\n";
	echo '<input name="file" type="file" style="display: none;" multiple>', "\n";
	echo '</span>', "\n";
	echo '</label>', "\n";
	echo '<input name="file" type="text" class="form-control" readonly>', "\n";
	echo '</div>', "\n";
	// upload button
	echo '<button class="btn btn-primary" title="upload new project" type="submit">', "\n";
	echo '<span class="glyphicon glyphicon-upload" type="submit"/>', "\n";
	echo '</button>', "\n";
	echo '</form>', "\n";
	echo '</div>', "\n";
}

function frontend_render_page_view_div($pid, $p, $n) {
	echo '<div id="page-view">', "\n";
	$page = backend_get_page($pid, $p, $n);
	if ($page === NULL) {
		frontend_render_error_div("could not load page");
	} else {
		frontend_render_page($page);
	}
	// echo '</div>', "\n";
	echo '</div>', "\n";
}

function frontend_render_page($page) {
	echo '<div id="page-view"';
	foreach ($page["lines"] as $line) {
		frontend_render_line($page, $line);
	}
	echo '</div>';
}

function frontend_render_line($page, $line) {
	$imgfile = $line["imgFile"];
	$lid = $line["id"];
	$text = $line["cor"];
	echo '<div class="line-view">';
	echo '<img src="', $imgfile, '"',
		// 'class="img-thumbnail"',
		'alt="line #', $lid, ', ', basename($imgfile), '"',
		'title="line #', $lid, ', ', basename($imgfile), '"',
		'width="', 6*strlen($text), '"',
		'height="auto"',
		' />';
	echo '<br/>';
	// echo '<iframe name="line-', $lid, '" style="display:none;"></iframe>';
	echo '<form action="page.php" method="post" id="line-', $lid, '">';
	echo '<input name="foo" type="text" size="', strlen($text), '" value="', $text, '" />';
	echo '<button type="submit"><span class="glyphicon glyphicon-upload" /></button>';
	echo '</button>';
	echo '</form>';
	echo '</div>';
}

function frontend_render_success_div($msg) {
	echo('<div class="row">');
	// echo('<div class="col-md-5 col-md-offset-1">');
	echo('<div class="alert alert-success" role="alert">');
	echo($msg);
	echo('</div>');
	// echo('</div>');
	echo('</div>');
}

function frontend_render_info_div($msg) {
	echo('<div class="row">');
	// echo('<div class="col-md-5 col-md-offset-1">');
	echo('<div class="alert alert-info" role="alert">');
	echo($msg);
	echo('</div>');
	// echo('</div>');
	echo('</div>');
}

function frontend_render_warning_div($msg) {
	echo('<div class="row">');
	// echo('<div class="col-md-5 col-md-offset-1">');
	echo('<div class="alert alert-warning" role="alert">');
	echo($msg);
	echo('</div>');
	// echo('</div>');
	echo('</div>');
}

function frontend_render_error_div($msg) {
	echo('<div class="row">');
	// echo('<div class="col-md-5 col-md-offset-1">');
	echo('<div class="alert alert-danger" role="alert">');
	echo($msg);
	echo('</div>');
	// echo('</div>');
	echo('</div>');
}
?>
