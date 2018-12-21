<?php
function utils_parse_http_cookie($xpr, $extras = array()) {
	$cookie = [
		"cookies" => array(),
		"extras"  => array(),
		"domain"  => "www.example.org",
		"path"    => "/",
		"expires" => 0,
	];
	preg_match_all('/([^=]*)=([^;]*);?\s*/', $xpr, $m);
	for ($i = 0; $i < count($m[1]); $i++) {
		$key = strtolower($m[1][$i]);
		$val = $m[2][$i];
		if (array_search($key, $extras) !== FALSE) {
			$cookie["extras"][$key] = $val;
		} else if ($key === "domain") {
			$cookie["domain"] = $val;
		} else if ($key === "path") {
			$cookie["path"] = $val;
		} else if ($key === "expires") {
			$cookie["expires"] = strtotime($val);
		} else {
			$cookie["cookies"][$key] = $val;
		}
	}
	return $cookie;
}
?>
