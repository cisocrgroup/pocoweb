<?php
require_once(dirname(dirname(__FILE__)) . "/config.php");
require_once(LIBRARY_PATH . "/utils.php");

class Api {
	public function __construct($url) {
		$this->url = $url;
		$this->json = NULL;
		$this->curl = curl_init($url);
		if ($this->curl === FALSE) {
			throw new Exception("Could not initialize curl handle");
		}
		global $config;
		$sid = $config["cookies"]["sid"];
		if (isset($_COOKIE[$sid])) {
			curl_setopt($this->curl, CURLOPT_COOKIE, $sid . "=" . $_COOKIE[$sid]);
		}
		curl_setopt($this->curl, CURLOPT_RETURNTRANSFER, TRUE);
		curl_setopt($this->curl, CURLOPT_HEADERFUNCTION, 'header_callback');
	}

	public function get_request() {
		$res = curl_exec($this->curl);
		if ($res === FALSE) {
			error_log("[Api] could not connect to: $this->url");
			return FALSE;
		}
		$this->json = json_decode($res, TRUE);
		return TRUE;
	}

	public function post_request($data) {
		curl_setopt($this->curl, CURLOPT_POSTFIELDS, json_encode($data));
		$res = curl_exec($this->curl);
		$this->json = NULL;
		if ($res === FALSE) {
			error_log("[Api] could not connect to: $this->url");
			return FALSE;
		}
		return TRUE;
	}

	public function delete_request() {
		curl_setopt($this->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		$res = curl_exec($this->curl);
		$this->json = NULL;
		if ($res === FALSE) {
			error_log("[Api] could not connect to: $this->url");
			return FALSE;
		} else {
			return TRUE;
		}
	}

	public function get_http_status_code() {
		return curl_getinfo($this->curl, CURLINFO_HTTP_CODE);
	}

	public function get_url() {
		return $this->url;
	}

	public function get_response() {
		if ($this->json === NULL) {
			return array();
		} else {
			return $this->json;
		}
	}

	public function __destruct() {
		if ($this->curl !== FALSE) {
			curl_close($this->curl);
		}
	}
}

function header_callback($curl, $header_line) {
	if (preg_match('/^Set-Cookie:\s*(.*)$/mi', $header_line, $m)) {
		$cookie = utils_parse_http_cookie($m[1]);
		foreach ($cookie["cookies"] as $key => $val) {
			if (!setcookie($key, $val, $cookie["expires"],
				$cookie["path"], $cookie["domain"])) {
				error_log("[Api] could not set cookie $key=$val; " .
					"expires=$cookie[expires]; $cookie[path]; $cookie[domain];");
			}
		}
	}
	return strlen($header_line);
}

?>
