if (typeof Object.create !== 'function') {
	Object.create = function(o) {
		var F = function() {};
		F.protoype = o;
		return new F();
	};
}

pcw.Api = {
	sid: null,
	post: null,
	url: "",
	method: "GET",
	acceptedStatuses: [200],
	formatRequest: function() {
		return this.method + " " + this.url + " [" + JSON.stringify(this.sid) + "]";
	},
	onError: function(status) {
		console.error(this.formatRequest() + " returned: " + status);
	},
	log: function(msg) {
		if (pcw.config.frontend.debug) {
			console.log("(Api) " + msg);
		}
	},
	accept: function(status) {
		for (var i = 0; i < this.acceptedStatuses.length; i++) {
			if (this.acceptedStatuses[i] === status) {
				return true;
			}
		}
		return false;
	},
	setupForGetVersion: function() {
		this.method = "GET";
		this.post = null;
		this.url = pcw.config.backend.restURL +
		    pcw.config.backend.routes.apiVersion;
		this.acceptedStatuses = [200];
	},
	setupForGetLoggedInUser: function() {
		this.method = "GET";
		this.post = null;
		this.url =
		    pcw.config.backend.restURL + pcw.config.backend.routes.login;
		this.acceptedStatuses = [200];
	},
	setupForCorrectWord: function(pid, p, lid, tid, c) {
		this.method = "POST";
		this.url = pcw.config.backend.restURL +
		    pcw.config.backend.routes.correctWord;
		this.url = this.url.replace('%d', pid);
		this.url = this.url.replace('%d', p);
		this.url = this.url.replace('%d', lid);
		this.url = this.url.replace('%d', tid);
		this.post = {correction: c};
		this.acceptedStatuses = [200];
	},
	setupForCorrectLine: function(pid, p, lid, c) {
		this.method = "POST";
		this.post = {correction: c};
		this.url = pcw.config.backend.restURL +
		    pcw.config.backend.routes.correctLine;
		this.url = this.url.replace('%d', pid);
		this.url = this.url.replace('%d', p);
		this.url = this.url.replace('%d', lid);
		this.acceptedStatuses = [200];
	},
	setupForGetConcordance: function(pid, q) {
		this.method = "GET";
		this.post = null;
		this.url =
		    pcw.config.backend.restURL + pcw.config.backend.routes.search;
		this.url = this.url.replace('%d', pid);
		this.url = this.url.replace('%s', encodeURI(q));
		this.url = this.url.replace('%d', '0');
		this.acceptedStatuses = [200];
	},
	setupForOrderProfile: function(pid) {
		this.method = "POST";
		this.post = {};
		this.url = pcw.config.backend.restURL +
		    pcw.config.backend.routes.orderProfile;
		this.url = this.url.replace('%d', pid);
		// 202 -> accepted
		this.acceptedStatuses = [202];
	},
	setupForSearchSuggestions: function(pid, q) {
		this.method = "GET";
		this.post = null;
		this.url = pcw.config.backend.restURL +
		    pcw.config.backend.routes.searchSuggestions;
		this.url = this.url.replace('%d', pid);
		this.url = this.url.replace('%s', encodeURI(q));
		// 202 -> accepted
		this.acceptedStatuses = [200];
	},
	setupForGetAllSuggestions: function(pid) {
		this.method = "GET";
		this.post = null;
		this.url = pcw.config.backend.restURL +
		    pcw.config.backend.routes.getAllSuggestions;
		this.url = this.url.replace('%d', pid);
		this.acceptedStatuses = [200]
	},
	setupForGetSuggestions: function(pid, p, lid, tid) {
		this.method = "GET";
		this.post = null;
		this.url = pcw.config.backend.restURL +
		    pcw.config.backend.routes.getSuggestions;
		this.url = this.url.replace('%d', pid);
		this.url = this.url.replace('%d', p);
		this.url = this.url.replace('%d', lid);
		this.url = this.url.replace('%d', tid);
		this.acceptedStatuses = [200];
	},
	setupForGetSuspiciousWords: function(pid, p, lid) {
		this.method = "GET";
		this.post = null;
		this.url = pcw.config.backend.restURL +
		    pcw.config.backend.routes.getSuspiciousWords;
		this.url = this.url.replace('%d', pid);
		this.url = this.url.replace('%d', p);
		this.url = this.url.replace('%d', lid);
		this.acceptedStatuses = [200];
	},
	setupForGetLanguages: function(url) {
		this.method = "GET";
		this.post = null;
		this.url = pcw.config.backend.restURL +
		    pcw.config.backend.routes.languages;
		this.url = this.url.replace('%s', encodeURI(url));
		this.acceptedStatuses = [200];
	},
	run: function(callback) {
		var http = new XMLHttpRequest();
		var that = this;
		http.onreadystatechange = function() {
			if (http.readyState === 4 && that.accept(http.status)) {
				that.log(
				    that.formatRequest() + " returned: " +
						http.status + " data: " +
						http.responseText);
				if (http.responseText.length > 0) {
					callback(
					    http.status,
					    JSON.parse(http.responseText));
				} else {
					callback(http.staus, {});
				}
			} else if (http.readyState === 4) {
				that.onError(http.status);
			}
		};
		if (this.sid != null) {
			this.url = this.url + "?auth=" + this.sid.auth;
		}
		http.open(this.method, this.url, true);
		http.setRequestHeader(
			"Content-Type", "application/json; charset=UTF-8");
		this.log(
			"sending request: " + this.formatRequest() + " data: " +
				JSON.stringify(this.post));
		if (this.method === "POST" && this.post !== null) {
			http.send(JSON.stringify(this.post));
		} else {
			http.send(null);
		}
	}
};
