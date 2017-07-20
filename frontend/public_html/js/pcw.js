// taken from:
// https://www.abeautifulsite.net/whipping-file-inputs-into-shape-with-bootstrap-3
$(function() {
	// We can attach the `fileselect` event to all file inputs on the page
	$(document)
	    .on('change', ':file', function() {
		    var input = $(this),
			numFiles =
			    input.get(0).files ? input.get(0).files.length : 1,
			label =
			    input.val().replace(/\\/g, '/').replace(/.*\//, '');
		    input.trigger('fileselect', [numFiles, label]);
	    });

	// We can watch for our custom `fileselect` event like this
	$(document)
	    .ready(function() {
		    $(':file')
			.on('fileselect', function(event, numFiles, label) {

				var input = $(this)
						.parents('.input-group')
						.find(':text'),
				    log = numFiles > 1 ?
				    numFiles + ' files selected' :
				    label;

				if (input.length) {
					input.val(log);
				} else {
					if (log) alert(log);
				}

			});
	    });

});

pcw.getIds = function(anchor) {
	var ids = anchor.split('-');
	for (var i = 0; i < ids.length; i++) {
		ids[i] = parseInt(ids[i]);
	}
	return ids;
};

pcw.toggleSelectionOfConcordanceTokens = function() {
	var items = document.getElementsByTagName("input");
	for (var i = 0; i < items.length; i++) {
		if (items[i].type === "checkbox") {
			items[i].checked = !items[i].checked;
		}
	}
};

pcw.setGlobalCorrectionSuggestion = function(suggestion) {
	var elem = document.getElementById("global-correction-suggestion");
	if (elem !== null) {
		elem.value = suggestion;
	}
};

pcw.setCorrectionSuggestionForAllSelectedConcordanceTokens = function() {
	var suggestion =
	    document.getElementById("global-correction-suggestion");
	var items = document.getElementsByTagName("input");
	const regex = /concordance-token-input-(\d+-\d+-\d+-\d+)/;
	if (suggestion != null) {
		for (var i = 0; i < items.length; i++) {
			var m = regex.exec(items[i].id);
			if (m !== null && m.length > 0) {
				if (document
					.getElementById(
					    "concordance-token-checkbox-" +
					    m[1])
					.checked) {
					items[i].value = suggestion.value;
				}
			}
		}
	}
};

pcw.getSidImpl = function() {
	const name = "pcw-sid=";
	var cookies = document.cookie.split(';');
	for (var i = 0; i < cookies.length; i++) {
		var c = cookies[i];
		while (c.charAt(0) == ' ') {
			c = c.substring(1, c.length);
		}
		if (c.indexOf(name) == 0) {
			return c.substring(name.length, c.length);
		}
		return null;
	}
};

pcw.getSid = function() {
	var sid = pcw.config.sid || pcw.getSidImpl();
	pcw.config.sid = sid;
	return sid;
};

pcw.setApiVersion = function() {
	api = Object.create(pcw.Api);
	api.setupForGetVersion();
	api.run(function(res) {
		const elem = document.getElementById('pcw-api-version');
		if (elem !== null) {
			elem.innerHTML = 'Api-Version: ' + res.version;
		}
	});
};

pcw.setLoggedInUserImpl = function(user) {
	console.log("user: " + JSON.stringify(user));
	var elem = document.getElementById('pcw-login');
	var li1 = document.createElement("li");
	var li2 = document.createElement("li");
	var p = document.createElement("p");
	var a = document.createElement("a");
	var t1 = document.createTextNode("Logged in as user: " + user.name);
	var t2 = document.createTextNode("Logout");
	p.className = "navbar-text";
	a.href = "logout.php";
	p.appendChild(t1);
	a.appendChild(t2);
	li1.appendChild(p);
	li2.appendChild(a);
	elem.parentNode.insertBefore(li1, elem);
	elem.parentNode.insertBefore(li2, elem);
	elem.parentNode.removeChild(elem);
	pcw.config.user = user;
};

pcw.setLoggedInUser = function() {
	var user = pcw.config.user || null;
	if (user === null) {
		var api = Object.create(pcw.Api);
		api.sid = pcw.getSid();
		api.setupForGetLoggedInUser();
		api.run(pcw.setLoggedInUserImpl);
	} else {
		pcw.setLoggedInUserImpl(user);
	}
};

pcw.setupCorrectedInputField = function(elem, res) {
	elem.value = res.cor;
	if (res.isFullyCorrected) {
		elem.className += " fully-corrected-line";
	} else if (res.isPartiallyCorrected) {
		elem.className += " partially-corrected-line";
	}
};

pcw.correctWord = function(anchor) {
	const ids = pcw.getIds(anchor);
	const inputid = "concordance-token-input-" + anchor;
	const correction = document.getElementById(inputid).value;
	var api = Object.create(pcw.Api);
	api.sid = pcw.getSid();
	api.setupForCorrectWord(ids[0], ids[1], ids[2], ids[3], correction);
	api.run(function(res) {
		var elem = document.getElementById(inputid);
		elem.value = res.cor;
		pcw.setupCorrectedInputField(elem, res);
	});
};

pcw.correctLine = function(anchor) {
	const ids = pcw.getIds(anchor);
	const correction = document.getElementById(anchor).value;
	var api = Object.create(pcw.Api);
	api.sid = pcw.getSid();
	api.setupForCorrectLine(ids[0], ids[1], ids[2], correction);
	api.run(function(res) {
		var elem = document.getElementById(anchor);
		pcw.setupCorrectedInputField(elem, res);
	});
};

pcw.correctAllLines = function(sid) {
	const regex = /(\d+)-(\d+)-(\d+)/;
	// iterate over all input nodes
	var items = document.getElementsByTagName("input");
	var ids = [];
	for (var i = 0; i < items.length; i++) {
		if (regex.test(items[i].id)) {
			ids = pcw.getIds(items[i].id);
			pcw.correctLine(items[i].id);
		}
	}
};

pcw.getSelectedWordFromInputElement = function(elem) {
	if (elem !== null && elem.tagName === "INPUT" && elem.type === "text") {
		var b = elem.selectionStart;
		var e = elem.selectionEnd;
		// we need unicode aware regexes to handle words like `ſunt` ...
		const regex = XRegExp('^\\PL*(.*?)\\PL*$');
		if ((e - b) > 0) {
			var m = regex.exec(elem.value.substring(b, e));
			return m[1];
		}
	}
	return null;
};

pcw.displayConcordance = function(anchor) {
	const pid = pcw.getIds(anchor)[0];
	const selection = pcw.getSelectedWordFromInputElement(
	    document.getElementById(anchor));
	if (selection !== null) {
		document.getElementById("concordance-search-label").value =
		    selection;
		var api = Object.create(pcw.Api);
		api.sid = pcw.getSid();
		api.setupForGetConcordance(pid, selection);
		api.run(function(res) {
			var searchButton =
			    document.getElementById('concordance-search-label');
			const n = res.nWords;
			var ocs = "occurrences";
			if (n === 1) {
				ocs = "occurrence";
			}
			searchButton.innerHTML = 'Show concordance of "' +
			    res.query + '" (' + n + ' ' + ocs + ')';
			searchButton.parentNode.setAttribute(
			    "href", "concordance.php?pid=" + pid + "&q=" +
				encodeURI(res.query));
		});
	}
};
