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
			curl_setopt($this->curl, CURLOPT_COOKIE, $sid .
				"=" . $_COOKIE[$sid]);
		}
		curl_setopt($this->curl, CURLOPT_RETURNTRANSFER, TRUE);
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
		$this->json = json_decode($res, TRUE);
		return TRUE;
	}

	public function put_request($data) {
        curl_setopt($this->curl, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($this->curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_setopt($this->curl, CURLOPT_POSTFIELDS,json_encode($data));
		$res = curl_exec($this->curl);
		$this->json = NULL;
		if ($res === FALSE) {
			error_log("[Api] could not connect to: $this->url");
			return FALSE;
		}
		$this->json = json_decode($res, TRUE);
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

	public function set_session_id($sid) {
        $this->url = $this->url . "?auth=" . $sid["auth"];
        curl_setopt($this->curl, CURLOPT_URL, $this->url);
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

?>
