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
	global $USER;
	if ($USER != NULL && $USER["admin"]) {
		frontend_render_upload_new_project_div();
		frontend_render_project_table_div(true);
	} else {
		frontend_render_project_table_div(false);
	}
}

function frontend_render_project_table_div($admin) {
	$api = backend_get_projects();
	$status = $api->get_http_status_code();
	if ($status != 200) {
		frontend_render_error_div("error could not load projects: $status");
	} else {
		$projects = $api->get_response();
		if (!isset($projects["books"])) {
			$projects["books"] = array();
		}
		$api = backend_get_users();
		$users = array("users" => array());
		if ($api->get_http_status_code() == 200) {
			$users = $api->get_response();
		}
		echo '<div class="container-fluid">', "\n";
		echo '<h2>Projects</h2>', "\n";
		frontend_render_project_table($admin, $projects, $users["users"]);
		echo '</div>', "\n";
	}
}

function frontend_render_project_table($admin, $projects, $users) {
	echo '<table class="table table-striped">', "\n";
	frontend_render_project_table_header();
	foreach ($projects["books"] as $project) {
		if ($admin) {
			frontend_render_admin_project_table_row($project, $users);
		} else {
			frontend_render_normal_user_project_table_row($project);
		}
	}
	echo '</table>', "\n";
}

function frontend_render_admin_project_table_row($project, $users) {
	$pid = $project["projectId"];
	echo '<tr>';
	echo '<td title="id">', $pid, '</td>';
	echo '<td title="author">', frontend_get_table_value($project["author"]), '</td>';
	echo '<td title="title">', frontend_get_table_value($project["title"]), '</td>';
	echo '<td title="year">', frontend_get_table_value($project["year"]), '</td>';
	echo '<td title="language">', frontend_get_table_value($project["language"]), '</td>';
	echo '<td title="pages">', frontend_get_table_value($project["pages"]), '</td>';
	echo '<td title="book">', frontend_get_table_value($project["isBook"]), '</td>';
	echo '<td title="actions">';
	echo '<div class="input-group">';
	// open project button
	echo '<div class="btn-group" role="group">', "\n";
	// echo '<span class="input-group-btn">';
	echo '<button class="btn btn-default"',
		' onclick="window.location.href=\'page.php?u=none&p=first&pid=', $pid, '\'"',
		' title="open project #', $pid, '">';
	echo '<span class="glyphicon glyphicon-open"/>';
	echo '</button>';
	// echo '</span>';
	// remove project button
	// echo '<span class="input-group-btn">';
	echo '<button class="btn btn-default"',
		' onclick="window.location.href=\'index.php?remove&pid=', $pid, '\'"',
		' title="remove project #', $pid, '">';
	echo '<span class="glyphicon glyphicon-remove"/>';
	echo '</button>';
	// echo '</span>';
	// download project button
	// echo '<span class="input-group-btn">';
	echo '<button class="btn btn-default"',
		' onclick="window.location.href=\'index.php?download&pid=', $pid, '\'"',
		' title="download project #', $pid, '">';
	echo '<span class="glyphicon glyphicon-download"/>';
	echo '</button>';
	echo '<button class="btn btn-default"',
		' onclick="pcw.orderProfile(', $pid, ');"',
		' title="order profile for project #', $pid, '">';
	echo '<span class="glyphicon glyphicon-glass"/>';
	echo '</button>';
	// echo '</span>';
	echo '</div>';
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
		echo '<select name="assign-user-name" class="form-control">', "\n";
		foreach ($users as $user) {
			if ($user["admin"]) { // skip admins
				continue;
			}
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

function frontend_render_normal_user_project_table_row($project) {
	$pid = $project["projectId"];
	echo '<tr>';
	echo '<td title="id">', $pid, '</td>';
	echo '<td title="author">', frontend_get_table_value($project["author"]), '</td>';
	echo '<td title="title">', frontend_get_table_value($project["title"]), '</td>';
	echo '<td title="year">', frontend_get_table_value($project["year"]), '</td>';
	echo '<td title="language">', frontend_get_table_value($project["language"]), '</td>';
	echo '<td title="pages">', frontend_get_table_value($project["pages"]), '</td>';
	echo '<td title="book">', frontend_get_table_value($project["isBook"]), '</td>';
	echo '<td title="actions">';
	echo '<div class="input-group">';
	// open project button
	echo '<span class="input-group-btn">';
	echo '<button class="btn btn-default"',
		' onclick="window.location.href=\'page.php?u=none&p=first&pid=', $pid, '\'"',
		' title="open project #', $pid, '">';
	echo '<span class="glyphicon glyphicon-ok"/>';
	echo '</button>';
	echo '</span>';
	// finish project button
	echo '<span class="input-group-btn">';
	echo '<button class="btn btn-default"',
		' onclick="window.location.href=\'index.php?finish&pid=', $pid, '\'"',
		' title="finish project #', $pid, '">';
	echo '<span class="glyphicon glyphicon-share"/>';
	echo '</button>';
	echo '</span>';
	echo '</div>';
	echo '</td></tr>', "\n";
}

function frontend_render_project_table_header() {
	echo '<tr>';
	echo '<th>#</th>';
	echo '<th>Author</th>';
	echo '<th>Title</th>';
	echo '<th>Year</th>';
	echo '<th>Language</th>';
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
	if ($file["size"] > $config["frontend"]["upload"]["max_size"]) {
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
	global $USER;
	if ($USER != NULL && $USER["admin"]) {
		frontend_render_create_new_user_div();
		frontend_render_users_table_div();
	} else {
		frontend_render_warning_div("You are not allowed to do user administration");
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
	echo '<label for="institute">Institute</label>', "\n";
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
	echo '<td title="id">', $user["id"], '</td>';
	echo '<td title="name">', frontend_get_table_value($user["name"]), '</td>';
	echo '<td title="email">', frontend_get_table_value($user["email"]), '</td>';
	echo '<td title="institute">', frontend_get_table_value($user["institute"]), '</td>';
	echo '<td title="admin">', frontend_get_table_value($user["admin"]), '</td>';
	if (!$user["admin"]) {
		echo '<td class="actions">';
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

function frontend_get_correction_class($obj) {
	if ($obj["isFullyCorrected"]) {
		return " fully-corrected";
	} else if ($obj["isPartiallyCorrected"]) {
		return " partially-corrected";
	}
	return "";
}

function frontend_render_page_view_div($pid, $p, $u, $post) {
	$api = backend_get_page($pid, $p);
	$status = $api->get_http_status_code();
	if ($status != 200) {
		frontend_render_error_div("error: could not load project #$pid, page #$p: $status");
	} else {
		$page = $api->get_response();
		echo '<div id="page-view" onload=\'pcw.setErrorsDropdown(', $pid, ')\'>', "\n";
		frontend_render_page_header_div($page);
		frontend_render_page_heading_div($page);
		frontend_render_page_div($page);
		frontend_render_page_correct_all_div($page);
		echo '</div>', "\n";
	}
}

function frontend_render_page_header_div($page) {
	$nextpageid = $page["nextPageId"];
	$prevpageid = $page["prevPageId"];
	$pid = $page["projectId"];
	echo '<nav class="navbar navbar-static-top" id="page-header" ',
		'data-spy="affix" data-offset-top="197"', '>', "\n";
	echo '<div class="container-fluid">', "\n";
	echo '<div class="collapse navbar-collapse">', "\n";
	// prev page and first page
	echo '<ul class="nav navbar-nav">', "\n";
	echo '<li><a href="page.php?u=none&p=first&pid=', $pid,
		'" title="got to first page">',
		'<span class="glyphicon glyphicon-fast-backward"/>',
		// 'Go to first page',
		'</a></li>';
	echo '<li><a href="page.php?u=none&p=', $prevpageid, '&pid=', $pid,
		'" title="got to previous page #', $prevpageid, '">',
		'<span class="glyphicon glyphicon-step-backward"/>',
		// 'Go to prev page',
		'</a></li>';
	echo '</ul>';
	// concordance search
	echo '<ul class="nav navbar-nav">', "\n";
	echo '<li><a href="#">',
		'<label id="concordance-search-label">Show concordance of \'\' (0 occurences)</label>',
		'</li></a>', "\n";
	echo '</ul>';
	// suggestions
	echo '<ul class="nav navbar-nav">', "\n";
	echo '<li class="dropdown">'; //  disabled">';
	echo '<a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" ',
		'aria-haspopup="true" aria-expanded="false">',
		'Correction suggestions<span class="caret"></span></a>';
        echo '<ul id="pcw-suggestions-dropdown" class="dropdown-menu">';
        echo '</ul>';
        echo '</li>';
	echo '</ul>';
	// error-patterns
	echo '<ul class="nav navbar-nav">', "\n";
	echo '<li class="dropdown">'; //  disabled">';
	echo '<a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" ',
		'aria-haspopup="true" aria-expanded="false">',
		'Error patterns<span class="caret"></span></a>';
        echo '<ul id="pcw-error-patterns-dropdown" class="dropdown-menu scrollable-menu">';
        echo '</ul>';
        echo '</li>';
	echo '</ul>';
	// error-tokens
	echo '<ul class="nav navbar-nav">', "\n";
	echo '<li class="dropdown">'; //  disabled">';
	echo '<a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" ',
		'aria-haspopup="true" aria-expanded="false">',
		'Error tokens<span class="caret"></span></a>';
        echo '<ul id="pcw-error-tokens-dropdown" class="dropdown-menu scrollable-menu">';
        echo '</ul>';
        echo '</li>';
	echo '</ul>';
	// nextpage and last page
	echo '<ul class="nav navbar-nav">', "\n";
	echo '<li><a href="page.php?u=none&p=', $nextpageid, '&pid=', $pid,
		'" title="got to next page #', $nextpageid, '">',
		'<span class="glyphicon glyphicon-step-forward"/>',
		// 'Go to next page',
		'</a></li>';
	echo '<li><a href="page.php?u=none&p=last&pid=', $pid,
		'" title="got to last page">',
		'<span class="glyphicon glyphicon-fast-forward"/>',
		// 'Go to last page',
		'</a></li>';
	echo '</ul>';
	echo '</div>';
	echo '</div>';
	echo '</nav>';
}

function frontend_render_page_heading_div($page) {
	echo '<div id="page-heading">', "\n";
	echo "<p><h2>Project #$page[projectId], page #$page[pageId]</h2></p>\n";
	echo '</div>', "\n";
}

function frontend_render_page_div($page) {
	echo '<div id="page-view">';
	// echo '<form method="post">';
	foreach ($page["lines"] as $line) {
		frontend_render_page_line_div($line);
	}
	echo '</div>';
}

function frontend_render_page_line_div($line) {
	global $SID;
	global $config;
	$lid = $line["lineId"];
	$p = $line["pageId"];
	$pid = $line["projectId"];
	$imgfile = $line["imgFile"];
	$file = basename($imgfile);
	$text = "line $lid, $file";
	$anchor = "$pid-$p-$lid";
	$d = $line["cor"];
	$inputclass = frontend_get_correction_class($line);
	echo '<div class="text-image-line" title="', $text, '">';
	echo '<a class="line-anchor" id="line-anchor-', $anchor, '"></a>';
	echo '<img src="', $imgfile, '"',
		'alt="', $text, '"',
		'title="', $text, '"',
		'width="auto"',
		'height="', $config["frontend"]["image"]["line_image_height"], '"',
		' />';
	echo '<br/>';
	echo '<div class="input-group">', "\n";
	echo '<input id="', $anchor, '" class="form-control', $inputclass,
		'" type="text" size="', strlen($d), '" value="', $d, '" ',
		'onclick=\'pcw.displayConcordance("', $anchor, '");\'',
		'/>', "\n";
	echo '<span class="input-group-btn">', "\n";
	echo '<button id="', $anchor, '-btn" class="btn btn-default" title="correct line #',
		$lid, '" onclick=\'pcw.correctLine("', $anchor, '");\' >', "\n";
	echo '<span class="glyphicon glyphicon-upload" />';
	echo '</button>', "\n";
	echo '</span>', "\n";
	echo '</div>', "\n";
	echo '</div>';
}

function frontend_render_page_correct_all_div($page) {
	global $SID;
	echo '<div id="correct-all-lines">', "\n";
	echo '<button id="correct-all-lines-btn" class="btn btn-default" title="correct all lines"',
		'onclick=\'pcw.correctAllLines("', $SID, '");\'>';
	echo 'Correct all';
	echo '</button', "\n";
	echo '</div>';
}

function frontend_get_subimage_div($src, $x, $y, $w, $h) {
	return "<div style=\"background-image:url($src);background-repeat:no-repeat;" .
		"background-position:-${x}px ${y}px;width:${w}px;height:${h}px;\"></div>";
}

function frontend_render_concordance_line_div($line, $word) {
	global $config;
	global $SID;
	$api = backend_get_split_images($word);
	$status = $api->get_http_status_code();
	if ($status != 200) {
		frontend_render_error_div("Could not get split images: server returned: $status");
		return;
	}
	$height = $config["frontend"]["image"]["line_image_height"];
	$linecor = preg_split('//u', $line["cor"], -1, PREG_SPLIT_NO_EMPTY);
	$wordcor = preg_split('//u', $word["cor"], -1, PREG_SPLIT_NO_EMPTY);
	$offset = $word["offset"];
	$link = "page.php?u=none&pid=$line[projectId]&p=$line[pageId]" .
		"#line-anchor-$line[projectId]-$line[pageId]-$line[lineId]";
	$anchor="$word[projectId]-$word[pageId]-$word[lineId]-$word[tokenId]";
	echo '<div class="text-image-line row">';
	$images = $api->get_response();
	// The three columns are divided 5,3,4.
	// This makes the left side of the line the biggest and gives enough space
	// for the token in the middle.
	// The right side is a little bit smaller but big enough to show the
	// emidate context of the selected tokens.
	echo '<div class="col-md-5 col-xs-5">';
	if ($images["leftImg"] != NULL) {
		echo '<a class="invisible=link" href="', $link, '">';
		echo '<img src="', $images["leftImg"],
			'" width="auto" height="', $height,
			'"/>', "\n";
		echo '<br/>';
		echo '<label>',
			implode("", array_slice($linecor, 0, $offset)),
			'</label>', "\n";
		echo '</a>', "\n";
	}
	echo '</div>';
	echo '<div class="col-md-3 col-xs-3">';
	if ($images["middleImg"] != NULL) {
		$inputclass = frontend_get_correction_class($word);
		echo '<a class="invisible=link" href="', $link, '">';
		echo '<img src="', $images["middleImg"],
			'" width="auto" height="', $height,
			'"/>', "\n";
		echo '</a>', "\n";
		echo '<br/>';
		echo '<div class="input-group">', "\n";
		// checkbox
		echo '<span class="input-group-addon">';
		echo '<input id="concordance-token-checkbox-', $anchor, '" ',
			'type="checkbox" aria-label="...">';
      		echo '</span>';
		// input
		echo '<input id="concordance-token-input-', $anchor, '" ',
			'class="form-control', $inputclass, '" type="text" value="',
			implode("", $wordcor), '" title="Correction" />', "\n";
		// dropdown
		echo '<div class="input-group-btn">', "\n";
		echo '<button type="button" class="btn btn-default dropdow-toggle" ',
			'data-toggle="dropdown" aria-haspopup="true" ',
			'aria-expanded="false" title="Correction suggestions">', "\n";
		echo '<span class="caret"></span>', "\n";
		echo '</button>', "\n";
		echo '<ul class="pull-right dropdown-menu">', "\n";
		echo '<li><a href="#">Correction #1</a></li>', "\n";
		echo '<li><a href="#">Correction #2</a></li>', "\n";
		echo '<li><a href="#">Correction #3</a></li>', "\n";
		echo '</ul>', "\n";
		// upload
		echo '<button type="button" class="btn btn-default" title="upload">', "\n";
		echo '<span class="glyphicon glyphicon-upload"></span>', "\n";
		echo '</button>', "\n";
		echo '</div>';
		echo '</div>';
	}
	echo '</div>';
	echo '<div class="col-md-4 col-xs-4">';
	if ($images["rightImg"] != NULL) {
		echo '<a class="invisible=link" href="', $link, '">';
		echo '<img src="', $images["rightImg"],
			'" width="auto" height="', $height,
			'"/>', "\n";
		echo '<br/>';
		echo '<label>',
			implode("", array_slice($linecor, $offset + count($wordcor))),
			'</label>', "\n";
		echo '</a>', "\n";
	}
	echo '</div>';
	echo '</div>';
}

function frontend_render_concordance_header_div() {
	echo '<nav class="navbar navbar-static-top" id="page-header" ',
		'data-spy="affix" data-offset-top="197"', '>', "\n";
	echo '<div class="container-fluid">', "\n";
	echo '<div class="collapse navbar-collapse">', "\n";
	echo '<ul class="nav navbar-nav">', "\n";
	echo '<li>';
	echo '<form class="navbar-form" action="javascript:void(0);" ',
		'onSubmit="">';
	echo '<div class="input-group">', "\n";
	echo '<span class="input-group-btn">';
	echo '<button id="foobar" class="btn btn-default" title="Toggle selection" ',
		'onclick="pcw.toggleSelectionOfConcordanceTokens()">';
	echo 'Toggle selection';
	echo '</button>';
	echo '</span>';
	echo '<input id="global-correction-suggestion" ',
		'class="form-control" title="Correction" ',
		'type="text" placeholder="Correction"/>';
	echo '<span class="input-group-btn">', "\n";
	echo '<button id="foobar" class="btn btn-default" title="Set correction" ',
		'onclick=\'pcw.setCorrectionSuggestionForAllSelectedConcordanceTokens();\'', ">\n";
	echo 'Set correction';
	echo '</button>';
	echo '</span>';
	echo '</div>';
	echo '</form>';
	echo '</li>';
	echo '</ul>';
	echo '</div>';
	echo '</div>';
	echo '</nav>';
}

function frontend_render_concordance_div($pid, $q, $isErrorPattern) {
	$api = backend_get_concordance($pid, $q, $isErrorPattern);
	$status = $api->get_http_status_code();
	if ($status == 200) {
		$matches = $api->get_response();
		if ($matches != NULL) {
			echo '<div id="concordance-view" class="container-fluid">', "\n";
			frontend_render_concordance_header_div();
			echo '<div id="concordance-heading">', "\n";
			echo "<p><h2>Concordance view for '", urldecode($q), "'</h2></p>\n";
			echo '</div>', "\n";
			if (isset($matches["matches"])) {
				foreach ($matches["matches"] as $match) {
					$line = $match["line"];
					foreach ($match["matches"] as $word) {
						frontend_render_concordance_line_div($line, $word);
					}
				}
			}
			echo '</div>', "\n";
		}
	} else {
		frontend_render_error_div("Error: backend returned: $status");
	}
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
