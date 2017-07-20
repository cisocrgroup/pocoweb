if (typeof Object.create !== 'function') {
	Object.create = function(o) {
		var F = function() {};
		F.protoype = o;
		return new F();
	};
}

PCW.Api = {
	sid: "",
	post: {},
	url: "",
	method: "GET",
	expectStatus: 200,
	formatRequest: function() {
		return this.method + " " + this.url + " [" + this.sid + "]";
	},
	setupForGetVersion: function() {
		this.method = "GET";
		this.post = {};
		this.url = PCW.config.backend.url +
		    PCW.config.backend.routes['api_version'];
		this.expectStatus = 200;
	},
	onError: function(status) {
		console.error(this.formatRequest() + " returned: " + status);
	},
	log: function(msg) {
		if (PCW.config.frontend.debug) {
			console.log("(Api) " + msg);
		}
	},
	run: function(callback) {
		var http = new XMLHttpRequest();
		var that = this;
		http.onreadystatechange = function() {
			if (http.readyState === 4 &&
			    http.status === that.expectStatus) {
				that.log(
				    that.formatRequest() + " returned: " +
				    http.status);
				callback(JSON.parse(http.responseText));
			} else if (http.readyState === 4) {
				that.onError(http.status);
			}
		};
		http.open(this.method, this.url, true);
		http.setRequestHeader(
		    "Content-type", "application/json; charset=UTF-8");
		http.setRequestHeader("Authorization", this.sid);
		this.log("sending request: " + this.formatRequest());
		http.send(this.post);
	}
};
