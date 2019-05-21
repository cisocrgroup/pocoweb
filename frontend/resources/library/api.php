<?php
require_once(dirname(dirname(__FILE__)) . "/config.php");
require_once(LIBRARY_PATH . "/utils.php");

class Api {
	public function __construct($url) {
		$this->url = $_SERVER['SERVER_NAME'] . $url;
        error_log("[Api] url: $this->url");
        error_log("[Api] server-name: $_SERVER[SERVER_NAME]");
		$this->json = NULL;
		$this->curl = curl_init($this->url);
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
        curl_setopt($this->curl, CURLOPT_HTTPHEADER, $this->get_default_header());
		$res = curl_exec($this->curl);
        $this->log("GET");
		if ($res === FALSE) {
			error_log("[Api] could not connect to: $this->url");
			return FALSE;
		}
		$this->json = json_decode($res, TRUE);
		return TRUE;
	}

	public function post_request($data) {
        curl_setopt($this->curl, CURLOPT_HTTPHEADER, $this->get_default_header());
		curl_setopt($this->curl, CURLOPT_POSTFIELDS, json_encode($data));
        $this->log("POST");
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
        curl_setopt($this->curl, CURLOPT_HTTPHEADER, $this->get_default_header());
        curl_setopt($this->curl, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($this->curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_setopt($this->curl, CURLOPT_POSTFIELDS,json_encode($data));
        $this->log("PUT");
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
        curl_setopt($this->curl, CURLOPT_HTTPHEADER, $this->get_default_header());
		curl_setopt($this->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        $this->log("DELETE");
		$res = curl_exec($this->curl);
		$this->json = NULL;
		if ($res === FALSE) {
			error_log("[Api] could not connect to: $this->url");
			return FALSE;
		} else {
			return TRUE;
		}
	}

    private function get_default_header() {
        $headers = array();
        $headers[] = 'User-Agent: pcw-php-api-client';
        $headers[] = 'Content-Type: application/json';
        return $headers;
    }

    private function log($req) {
        error_log("[Api] sending $req $this->url");
    }

	public function set_session_id($sid) {
        $pos = strrpos($this->url, "?");
        if ($pos === FALSE) {
            $this->url = $this->url . "?auth=" . $sid["auth"];
        } else {
            $this->url = $this->url . "&auth=" . $sid["auth"];
        }
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
