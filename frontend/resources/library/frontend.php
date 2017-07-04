<?php

require_once(dirname(dirname(__FILE__)) . "/config.php");
require_once(LIBRARY_PATH . "/backend.php");

function frontend_render_login_div() {
	echo '<div class="container-fluid">';
	echo '<form class="form-horizontal" method="post">';
	echo '<div class="form-group">';
	echo '<label for="name">User name</label>';
	echo '<input name="name" type="text" class="form-control" placeholder="Username"/>';
	echo '</div>';
	echo '<div class="form-group">';
	echo '<label for="pass">Password</label>';
	echo '<input name="pass" type="password" class="form-control" placeholder="Password"/>';
	echo '</div>';
	echo '<button type="submit" class="btn btn-primary">Login</button>';
	echo '</form>';
	echo '</div>';
}

function frontend_render_projects_div() {
	frontend_render_upload_new_project_div();
	frontend_render_project_table_div();
}

function frontend_render_project_table_div() {
	$api = backend_get_projects();
	$status = $api->get_http_status_code();
	if ($status != 200) {
		frontend_render_error_div("error could not load projects: $status");
	} else {
		$projects = $api->get_response();
		$api = backend_get_users();
		$users = array();
		if ($api->get_http_status_code() == 200) {
			$users = $api->get_response();
		}
		echo '<div class="container-fluid">', "\n";
		echo '<h2>Projects</h2>', "\n";
		frontend_render_project_table($projects, $users["users"]);
		echo '</div>', "\n";
	}
}

function frontend_render_project_table($projects, $users) {
	echo '<table class="table table-striped">', "\n";
	frontend_render_project_table_header();
	foreach ($projects["books"] as $project) {
		frontend_render_project_table_row($project, $users);
	}
	echo '</table>', "\n";
}

function frontend_render_project_table_row($project, $users) {
	$pid = $project["projectId"];
	echo '<tr>';
	echo '<td>', $pid, '</td>';
	echo '<td>', frontend_get_table_value($project["author"]), '</td>';
	echo '<td>', frontend_get_table_value($project["title"]), '</td>';
	echo '<td>', frontend_get_table_value($project["year"]), '</td>';
	echo '<td>', frontend_get_table_value($project["language"]), '</td>';
	echo '<td>', frontend_get_table_value($project["pages"]), '</td>';
	echo '<td>', frontend_get_table_value($project["isBook"]), '</td>';
	echo '<td>';
	echo '<div class="input-group">';
	// open project button
	echo '<span class="input-group-btn">';
	echo '<button class="btn btn-default"',
		' onclick="window.location.href=\'page.php?u=none&p=first&pid=', $pid, '\'"',
		' title="open project #', $pid, '">';
	echo '<span class="glyphicon glyphicon-ok"/>';
	echo '</button>';
	echo '</span>';
	// delete project button
	echo '<span class="input-group-btn">';
	echo '<button class="btn btn-default"',
		' onclick="window.location.href=\'index.php?delete&pid=', $pid, '\'"',
		' title="delete project #', $pid, '">';
	echo '<span class="glyphicon glyphicon-remove"/>';
	echo '</button>';
	echo '</span>';
	// download project button
	echo '<span class="input-group-btn">';
	echo '<button class="btn btn-default"',
		' onclick="window.location.href=\'index.php?download&pid=', $pid, '\'"',
		' title="download project #', $pid, '">';
	echo '<span class="glyphicon glyphicon-download"/>';
	echo '</button>';
	echo '</span>';
	// other buttons
	if ($project["isBook"]) {
		echo '<form method="post" class="form-inline" ',
			'action="index.php?split&pid=', $pid, '">', "\n";
		echo '<div class="form-group">';
		echo '<input name="split-n" size="3" type="number" min="1" max="100" ',
			'step="1" value="10" class="form-control"/>', "\n";
		echo '</div>';
		echo '<div class="form-group">';
		echo '<span class="input-group-addon">';
		echo '<input name="random" title="random" type="checkbox"/>';
		echo '</span>';
		echo '<span class="input-group-btn">';
		echo '<button title="split project #', $pid,
			'" class="btn btn-default" type="submit">';
		echo '<span class="glyphicon glyphicon-resize-full"/>';
		echo '</button>';
		echo '</span>';
		echo '</div>';
		echo '</div>', "\n";
		echo '</form>', "\n";
		echo '</div>', "\n";
	} else {
		echo '<form method="post" class="form-inline" ',
			'action="index.php?assign&pid=', $pid, '">', "\n";
		echo '<div class="form-group">';
		echo '<select class="form-control">', "\n";
		foreach ($users as $user) {
			echo '<option value="', $user["name"], '">', $user["name"], '</option>', "\n";
		}
		echo '</select>', "\n";
		echo '</div>';
		echo '<div class="form-group">';
		echo '<span class="input-group-btn">';
		echo '<button title="assign project #', $pid,
			' to user" class="btn btn-default" type="submit">';
		echo '<span class="glyphicon glyphicon-share"/>';
		echo '</button>';
		echo '</span>';
		echo '</div>';
		echo '</div>', "\n";
		echo '</form>', "\n";
		echo '</div>', "\n";
	}
	echo '</td></tr>', "\n";
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
	return '<button class="btn btn-default" onclick="window.location.href=\'page.php?u=none&p=first&pid='
		. $id . '\'" title="' . $msg . ' #' . $id . '">'
		. '<span class="' . $class . '"/>'
		. '</button>';
}

function frontend_get_table_value($val) {
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
	echo '<div id="upload-project" class="container-fluid">', "\n";
	echo '<h2>Upload new project</h2>', "\n";
	echo '<form action="index.php?upload" method="post" enctype="multipart/form-data">', "\n";
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
	echo '<input name="year" type="number" min="0" max="2099" ',
		'step="1" value="2017" class="form-control"/>', "\n";
	echo '</div>', "\n";
	// Language
	echo '<div class="form-group">', "\n";
	echo '<label for="language">Language</label>', "\n";
	echo '<input name="language" type="text" placeholder="Language" class="form-control"/>', "\n";
	echo '</div>', "\n";
	// upload file
	echo '<div class="form-group">', "\n";
	echo '<label for="archive-upload">Upload project</label>', "\n";
	echo '<input type="file" id="archive-upload" name="archive" />', "\n";
	echo '</div>', "\n";
	// upload button
	echo '<button class="btn btn-primary" title="upload new project" type="submit">', "\n";
	echo 'Upload';
	echo '</button>', "\n";
	echo '</form>', "\n";
	echo '</div>', "\n";
}

function frontend_upload_project_archive($post, $file) {
	global $config;
	if ($file["error"] != UPLOAD_ERR_OK) {
		frontend_render_error_div("Could not upload archive: error: $file[error]");
		return;
	}
	if ($file["size"] > $config["backend"]["upload"]["max_size"]) {
		frontend_render_error_div("Could not upload archive: file too big");
		return;
	}
	if ($file["type"] != "application/zip") {
		frontend_render_error_div("Could not upload archive: not a zip file");
		return;
	}
	if (!file_exists($file["tmp_name"])) {
		frontend_render_error_div("Could not upload archive: upload file does not exist");
	}
	if (!chmod($file["tmp_name"], 0755)) {
		frontend_render_error_div("Could not upload archive: could publish upload file");
	}
	$api = backend_upload_project($post, $file["name"], $file["tmp_name"]);
	$status = $api->get_http_status_code();
	if ($status != 201) {
		frontend_render_error_div("Could not upload archive: backend returned $status");
	} else {
		frontend_render_success_div("Successfully uploaded new project");
	}
}

function frontend_render_users_div() {
	global $user;
	if ($user["admin"]) {
		frontend_render_create_new_user_div();
		frontend_render_users_table_div();
	}
}

function frontend_render_create_new_user_div() {
	echo '<div id="create-new-user" class="container-fluid">', "\n";
	echo '<h2>Create new user</h2>', "\n";
	echo '<form action="users.php?create" method="post">', "\n";
	// Name
	echo '<div class="form-group">', "\n";
	echo '<label for="name">Username</label>', "\n";
	echo '<input name="name" type="text" placeholder="Username" class="form-control"/>', "\n";
	echo '</div>', "\n";
	// Email
	echo '<div class="form-group">', "\n";
	echo '<label for="email">Email</label>', "\n";
	echo '<input name="email" type="email" placeholder="Email" class="form-control"/>', "\n";
	echo '</div>', "\n";
	// Institute
	echo '<div class="form-group">', "\n";
	echo '<label for="institute">Insitute</label>', "\n";
	echo '<input name="institute" type="text" placeholder="Institute" class="form-control"/>', "\n";
	echo '</div>', "\n";
	// Password 1
	echo '<div class="form-group">', "\n";
	echo '<label for="pass">Password</label>', "\n";
	echo '<input name="pass" type="password" placeholder="Password" class="form-control"/>', "\n";
	echo '</div>', "\n";
	// Password 2
	echo '<div class="form-group">', "\n";
	echo '<label for="pass2">Password (retype)</label>', "\n";
	echo '<input name="pass2" type="password" placeholder="Password" class="form-control"/>', "\n";
	echo '</div>', "\n";
	// Admin
	echo '<div class="checkbox">', "\n";
	echo '<label>', "\n";
	echo '<input name="admin" type="checkbox"/>Admin', "\n";
	echo '</label>', "\n";
	echo '</div>', "\n";
	// upload button
	echo '<button class="btn btn-primary" title="create new user" type="submit">', "\n";
	echo 'Create', "\n";
	echo '</button>', "\n";
	echo '</form>', "\n";
	echo '</div>', "\n";
}

function frontend_render_users_table_div() {
	$api = backend_get_users();
	$status = $api->get_http_status_code();
	if ($status != 200) {
		frontend_render_error_div("error: could not load users: $status");
	} else {
		$users = $api->get_response();
		echo '<div class="container-fluid">', "\n";
		echo '<h2>Users</h2>', "\n";
		frontend_render_users_table($users);
		echo '</div>', "\n";
	}
}

function frontend_render_users_table($users) {
	echo '<table class="table table-striped">', "\n";
	frontend_render_users_table_header();
	foreach ($users["users"] as $user) {
		frontend_render_users_table_row($user);
	}
	echo '</table>', "\n";
}

function frontend_render_users_table_row($user) {
	echo '<tr>';
	echo '<td>', $user["id"], '</td>';
	echo '<td>', frontend_get_table_value($user["name"]), '</td>';
	echo '<td>', frontend_get_table_value($user["email"]), '</td>';
	echo '<td>', frontend_get_table_value($user["institute"]), '</td>';
	echo '<td>', frontend_get_table_value($user["admin"]), '</td>';
	if (!$user["admin"]) {
		echo '<td>';
		echo '<button onclick="window.location.href=\'users.php?delete&uid=',
			$user["id"], '\'" title="delete user #', $user["id"], '">', "\n";
		echo '<span class="glyphicon glyphicon-remove" />', "\n";
		echo '</button>', "\n";
	} else {
		echo '<td />', "\n";
	}
}

function frontend_render_users_table_header() {
	echo '<tr>';
	echo '<th>#</th>';
	echo '<th>Name</th>';
	echo '<th>Email</th>';
	echo '<th>Institute</th>';
	echo '<th>Admin</th>';
	echo '<th>Actions<th/>';
	echo '</tr>', "\n";
}

function frontend_render_page_view_div($pid, $p, $u, $post) {
	if (isset($post["lines"])) {
		frontend_update_lines($u, $post["lines"]);
	}
	$api = backend_get_page($pid, $p);
	$status = $api->get_http_status_code();
	if ($status != 200) {
		frontend_render_error_div("error: could not load project #$pid, page #$p: $status");
	} else {
		$page = $api->get_response();
		echo '<div id="page-view">', "\n";
		frontend_render_page_header($page);
		frontend_render_page_heading($page);
		frontend_render_page($page);
		echo '</div>', "\n";
	}
}

function frontend_update_lines($u, $lines) {
	if ($u === "none") {
		return;
	}
	$oklines = "";
	$errorlines = "";
	foreach ($lines as $key => $val) {
		if (preg_match('/(\d+)-(\d+)-(\d+)/', $key, $m)) {
			if ($u === "all" || $u === $key) {
				$api = backend_correct_line($m[1], $m[2], $m[3], $val);
				$status = $api->get_http_status_code();
				if ($status == 200) {
					$oklines .= "#$m[3] ";
				} else {
					$errorlines .= "#$m[3]($status) ";
				}
			}
		}
	}
	if (strlen($errorlines) > 0) {
		frontend_render_error_div("Error updating lines: $errorlines");
	}
	if (strlen($oklines) > 0) {
		frontend_render_success_div("Successfully updated lines: $oklines");
	}
}

function frontend_render_page_header($page) {
	$nextpageid = $page["nextPageId"];
	$prevpageid = $page["prevPageId"];
	$pid = $page["projectId"];
	echo '<div id="page-header" class="navbar navbar-nav" data-spy="affix" data-offset-top="141">', "\n";
	// navigation buttons
	frontend_render_page_navigation_buttons($pid, $prevpageid, TRUE);
	frontend_render_page_navigation_buttons($pid, $nextpageid, FALSE);
	// search
	echo '<div class="input-group centered">', "\n";
	echo '<span class="input-group-btn">', "\n";
	echo '<button class="btn btn-default centered" type="button">', "\n";
	echo '<span class="glyphicon glyphicon-search centered"/>', "\n";
	echo '</button>', "\n";
	echo '</span>', "\n";
	echo '<input type="text" class="form-control centered" placeholder="Search"/>', "\n";
	echo '</div>', "\n";
	echo '</div>', "\n";
}

function frontend_render_page_navigation_buttons($pid, $p, $left) {
	$btnclass = "btn btn-default";
	$spanclass = "glyphicon";
	$ospanclass = "glyphicon";
	$title = "go to page #$p";
	$dir = "";
	if ($left) {
		$btnclass .= " pull-left";
		$spanclass .= ' glyphicon-step-backward';
		$ospanclass .= ' glyphicon-fast-backward';
		$dir = "first";
	} else {
		$btnclass .= " pull-right";
		$spanclass .= ' glyphicon-step-forward';
		$ospanclass .= ' glyphicon-fast-forward';
		$dir = "last";
	}
	if ($p <= 0) {
		$btnclass .= " disabled";
	}
	echo '<button class="', $btnclass, '" type="button" ',
		'onclick="window.location.href=\'page.php?u=none&p=',
		$dir, '&pid=', $pid, '\'"',
		'title="go to ', $dir, ' page">', "\n";
	echo '<span class="', $ospanclass, '"/>', "\n";
	echo '</button>', "\n";
	echo '<button class="', $btnclass, '" type="button" ',
		'onclick="window.location.href=\'page.php?u=none&p=',
		$p, '&pid=', $pid, '\'"',
		'title="', $title, '">', "\n";
	echo '<span class="', $spanclass, '"/>', "\n";
	echo '</button>', "\n";
}

function frontend_render_page_heading($page) {
	echo '<div id="page-heading">', "\n";
	echo "<p><h2>Project #$page[projectId], page #$page[id]</h2></p>\n";
	echo '</div>', "\n";
}

function frontend_render_page($page) {
	echo '<div id="page-view">';
	echo '<form method="post">';
	foreach ($page["lines"] as $line) {
		frontend_render_page_line_div($page["projectId"], $page["id"], $line);
	}
	echo '<button class="btn btn-primary" type="submit" title="', "upload page #$page[id]",
		'" formaction="', "page.php?u=all&p=$page[id]&pid=$page[projectId]", '">';
	echo '<span class="glyphicon glyphicon-upload"/>';
	echo '</button>';
	echo '</form>';
	echo '</div>';
}

function frontend_render_page_line_div($pid, $p, $line) {
	$lid = $line["id"];
	$imgfile = $line["imgFile"];
	$file = basename($imgfile);
	$text = "line $lid, $file";
	$anchor = "$pid-$p-$lid";
	$d = $line["cor"];
	$inputclass = '';
	if ($line["isCorrected"]) {
		$inputclass = 'class="corrected-line"';
	}
	echo '<div class="line-view" title="', $text, '">';
	// echo '<a class="line-anchor" id="', $anchor, '"></a>';
	echo '<img src="', $imgfile, '"',
		'alt="', $text, '"',
		'title="', $text, '"',
		'width="auto"',
		'height="25"',
		' />';
	echo '<br/>';
	echo '<input name="lines[', $anchor, ']" type="text" size="', strlen($d), '" value="', $d, '"',
		$inputclass, '/>';
	echo '<button class="btn btn-default" title="', "upload line #$lid", '" type="submit" formaction="',
		"page.php?u=$anchor&p=$p&pid=$pid", '">';
	echo '<span class="glyphicon glyphicon-upload" />';
	echo '</button>';
	echo '</div>';
}

function frontend_render_success_div($msg) {
	echo('<div class="container-fluid">');
	echo('<div class="alert alert-success" role="alert">');
	echo($msg);
	echo('</div>');
	echo('</div>');
}

function frontend_render_info_div($msg) {
	echo('<div class="container-fluid">');
	echo('<div class="alert alert-info" role="alert">');
	echo($msg);
	echo('</div>');
	echo('</div>');
}

function frontend_render_warning_div($msg) {
	echo('<div class="container-fluid">');
	echo('<div class="alert alert-warning" role="alert">');
	echo($msg);
	echo('</div>');
	echo('</div>');
}

function frontend_render_error_div($msg) {
	echo('<div class="container-fluid">');
	echo('<div class="alert alert-danger" role="alert">');
	echo($msg);
	echo('</div>');
	echo('</div>');
}
?>
