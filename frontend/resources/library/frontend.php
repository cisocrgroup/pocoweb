<?php

require_once(dirname(dirname(__FILE__)) . "/config.php");
require_once(LIBRARY_PATH . "/backend.php");

function frontend_render_login_div() {
	echo('<div class="row">');
	echo('<form action="login.php" method="post">');
	echo('<input name="name" type="text" class="form-control" placeholder="Username" />');
	echo('<input name="pass" type="password" class="form-control" placeholder="Password" />');
	echo('<input type="submit" value="Login"/>');
	echo('</form>');
	echo('</div>');
}

function frontend_render_projects_div() {
		frontend_render_home_div();
		frontend_render_upload_new_project_div();
		frontend_render_project_table_div();
}

function frontend_render_home_div() {
	echo('<div id="home">');
	echo('<h1>Home</h1>');
	echo('</div>');
}

function frontend_render_project_table_div() {
	$projects = backend_get_projects();
	if ($projects === NULL) {
		frontend_render_error_div("internal error: could not load projects");
	} else {
		echo '<h2>Projects</h2>', "\n";
		frontend_render_project_table($projects);
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
	return '<button onclick="window.location.href=\'page.php?u=none&p=first&pid='
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
	echo '<input name="year" type="number" min="0" max="2099" ',
		'step="1" value="2017" class="form-control"/>', "\n";
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

function frontend_render_page_view_div($pid, $p, $u, $post) {
	if (isset($post["lines"])) {
		frontend_update_lines($u, $post["lines"]);
	}
	echo '<div id="page-view">', "\n";
	$page = backend_get_page($pid, $p);
	if ($page === NULL) {
		frontend_render_error_div("could not load project #$pid, page #$p");
	} else {
		frontend_render_page_header($page);
		frontend_render_page_heading($page);
		frontend_render_page($page);
	}
	echo '</div>', "\n";
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
				$status = backend_correct_line($m[1], $m[2], $m[3], $val);
				if ($status === 200) {
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
		frontend_render_page_line($page["projectId"], $page["id"], $line);
	}
	echo '<button class="btn btn-primary" type="submit" title="', "upload page #$page[id]",
		'" formaction="', "page.php?u=all&p=$page[id]&pid=$page[projectId]", '">';
	echo '<span class="glyphicon glyphicon-upload"/>';
	echo '</button>';
	echo '</form>';
	echo '</div>';
}

function frontend_render_page_line($pid, $p, $line) {
	$lid = $line["id"];
	$imgfile = $line["imgFile"];
	$file = basename($imgfile);
	$text = "line $lid, $file";
	$anchor = "$pid-$p-$lid";
	$d = $line["cor"];
	$inputstyle = '';
	if ($line["isCorrected"]) {
		$inputstyle = 'style="background-color: lightgrey;"';
	}
	echo '<div class="line-view" title="', $text, '">';
	// echo '<a class="line-anchor" id="', $anchor, '"></a>';
	echo '<img src="', $imgfile, '"',
		'alt="', $text, '"',
		'title="', $text, '"',
		'width="', 6*strlen($d), '"',
		'height="auto"',
		' />';
	echo '<br/>';
	echo '<input name="lines[', $anchor, ']" type="text" size="', strlen($d), '" value="', $d, '"',
		$inputstyle, '/>';
	echo '<button class="btn btn-default" title="', "upload line #$lid", '" type="submit" formaction="',
		"page.php?u=$anchor&p=$p&pid=$pid", '">';
	echo '<span class="glyphicon glyphicon-upload" />';
	echo '</button>';
	echo '</div>';
}

function frontend_render_success_div($msg) {
	echo('<div class="row">');
	echo('<div class="alert alert-success" role="alert">');
	echo($msg);
	echo('</div>');
	echo('</div>');
}

function frontend_render_info_div($msg) {
	echo('<div class="row">');
	echo('<div class="alert alert-info" role="alert">');
	echo($msg);
	echo('</div>');
	echo('</div>');
}

function frontend_render_warning_div($msg) {
	echo('<div class="row">');
	echo('<div class="alert alert-warning" role="alert">');
	echo($msg);
	echo('</div>');
	echo('</div>');
}

function frontend_render_error_div($msg) {
	echo('<div class="row">');
	echo('<div class="alert alert-danger" role="alert">');
	echo($msg);
	echo('</div>');
	echo('</div>');
}
?>
