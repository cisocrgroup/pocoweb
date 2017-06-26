<?php
require_once(dirname(dirname(__FILE__)) . "/config.php");
require_once(LIBRARY_PATH . "/utils.php");

class Api {
	public function __construct($url) {
		$this->url = $url;
		$this->curl = curl_init($url);
		if ($this->curl !== FALSE) {
			global $config;
			$sid = $config["cookies"]["sid"];
			if (isset($_COOKIE[$sid])) {
				curl_setopt($this->curl, CURLOPT_COOKIE, $sid . "=" . $_COOKIE[$sid]);
			}
			curl_setopt($this->curl, CURLOPT_RETURNTRANSFER, TRUE);
			curl_setopt($this->curl, CURLOPT_HEADERFUNCTION, 'header_callback');
		} else {
			error_log("[Api] could not curl_init($this->url)");
		}
	}

	public function get_request() {
		if ($this->curl === FALSE) {
			error_log("[Api] invalid curl handle: $this->url");
			return NULL;
		}
		$res = curl_exec($this->curl);
		if ($res === FALSE) {
			error_log("[Api] could not connect to: $this->url");
			return NULL;
		}
		$json = json_decode($res, TRUE);
		if ($json === NULL) {
			error_log("[Api] invalid json reply from url: $this->url");
			return NULL;
		}
		return $json;
	}

	public function post_request($post_data) {
		if ($this->curl === FALSE) {
			error_log("[Api] invalid curl handle: $this->url");
			return NULL;
		}
		curl_setopt($this->curl, CURLOPT_POSTFIELDS, $post_data);
		$res = curl_exec($this->curl);
		if ($res === FALSE) {
			error_log("[Api] could not connect to: $this->url");
			return NULL;
		}
		return json_decode($res, TRUE);
	}

	public function get_http_status_code() {
		if ($this->curl === FALSE) {
			error_log("[Api] invalid curl handle: $this->url");
			return "500";
		}
		return curl_getinfo($this->curl, CURLINFO_HTTP_CODE);
	}

	public function __destruct() {
		if ($this->curl !== FALSE) {
			curl_close($this->curl);
		}
	}
}

function header_callback($curl, $header_line) {
	// echo 'header callback: ', $header_line, 'm[1]: ', "<br/>\n";
	if (preg_match('/^Set-Cookie:\s*(.*)$/mi', $header_line, $m)) {
		$cookie = utils_parse_http_cookie($m[1]);
		foreach ($cookie["cookies"] as $key => $val) {
			// echo("[Api] could not set cookie $key=$val; expires=$cookie[expires]; $cookie[path]; domain=$cookie[domain];<br/>");
			if (!setcookie($key, $val, $cookie["expires"],
				$cookie["path"], $cookie["domain"])) {
				error_log("[Api] could not set cookie $key=$val; expires=$cookie[expires]; $cookie[path]; $cookie[domain];");
			} /*else {
				error_log("[Api] setting cookie $key=$val; expires=$cookie[expires]; $cookie[path]; $cookie[domain];");
				}*/
		}
	}
	return strlen($header_line);
}

?>
