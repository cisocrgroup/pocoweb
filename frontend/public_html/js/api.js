if (typeof Object.create !== 'function') {
	Object.create = function(o) {
		var F = function() {};
		F.protoype = o;
		return new F();
	};
}

pcw.Api = {
	sid: "",
	post: null,
	url: "",
	method: "GET",
	expectStatus: 200,
	formatRequest: function() {
		return this.method + " " + this.url + " [" + this.sid + "]";
	},
	onError: function(status) {
		console.error(this.formatRequest() + " returned: " + status);
	},
	log: function(msg) {
		if (pcw.config.frontend.debug) {
			console.log("(Api) " + msg);
		}
	},
	setupForGetVersion: function() {
		this.method = "GET";
		this.post = null;
		this.url = pcw.config.backend.url +
		    pcw.config.backend.routes['api_version'];
		this.expectStatus = 200;
	},
	setupForCorrectWord: function(pid, p, lid, tid, c) {
		this.method = "POST";
		this.post = {
			projectId: pid,
			pageId: p,
			lineId: lid,
			tokenId: tid,
			correction: c
		};
		this.url = pcw.config.backend.url +
		    pcw.config.backend.routes['correct_line'];
		this.expectStatus = 200;
	},
	setupForCorrectLine: function(pid, p, lid, c) {
		this.method = "POST";
		this.post =
		    {projectId: pid, pageId: p, lineId: lid, correction: c};
		this.url = pcw.config.backend.url +
		    pcw.config.backend.routes['correct_line'];
		this.expectStatus = 200;
	},
	setupForGetConcordance: function(pid, q) {
		this.method = "GET";
		this.post = null;
		this.url =
		    pcw.config.backend.url + pcw.config.backend.routes.search;
		this.url = this.url.replace('%d', pid);
		this.url = this.url.replace('%s', q);
		this.expectStatus = 200;
	},
	run: function(callback) {
		var http = new XMLHttpRequest();
		var that = this;
		http.onreadystatechange = function() {
			if (http.readyState === 4 &&
			    http.status === that.expectStatus) {
				that.log(
				    that.formatRequest() + " returned: " +
				    http.status + " data: " +
				    http.responseText);
				callback(JSON.parse(http.responseText));
			} else if (http.readyState === 4) {
				that.onError(http.status);
			}
		};
		http.open(this.method, this.url, true);
		http.setRequestHeader(
		    "Content-type", "application/json; charset=UTF-8");
		http.setRequestHeader("Authorization", this.sid);
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
