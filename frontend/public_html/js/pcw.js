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

PCW.getIds = function(anchor) {
	var ids = anchor.split('-');
	for (var i = 0; i < ids.length; i++) {
		ids[i] = parseInt(ids[i]);
	}
	return ids;
};

PCW.toggleSelectionOfConcordanceTokens = function() {
	var items = document.getElementsByTagName("input");
	for (var i = 0; i < items.length; i++) {
		if (items[i].type === "checkbox") {
			items[i].checked = !items[i].checked;
		}
	}
};

PCW.setGlobalCorrectionSuggestion = function(suggestion) {
	var elem = document.getElementById("global-correction-suggestion");
	if (elem !== null) {
		elem.value = suggestion;
	}
};

PCW.setCorrectionSuggestionForAllSelectedConcordanceTokens = function() {
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

PCW.getSidImpl = function() {
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

PCW.getSid = function() {
	var sid = PCW.config.sid || PCW.getSidImpl();
	PCW.config.sid = sid;
	return sid;
};

PCW.setApiVersion = function() {
	api = Object.create(PCW.Api);
	api.setupForGetVersion();
	api.run(function(res) {
		const elem = document.getElementById('pcw-api-version');
		if (elem !== null) {
			elem.innerHTML = 'Api-Version: ' + res.version;
		}
	});
};

PCW.setupCorrectedInputField = function(elem, res) {
	elem.value = res.cor;
	if (res.isFullyCorrected) {
		elem.className += " fully-corrected-line";
	} else if (res.isPartiallyCorrected) {
		elem.className += " partially-corrected-line";
	}
};

PCW.correctWord = function(anchor) {
	const ids = PCW.getIds(anchor);
	const inputid = "concordance-token-input-" + anchor;
	const correction = document.getElementById(inputid).value;
	var api = Object.create(PCW.Api);
	api.sid = PCW.getSid();
	api.setupForCorrectWord(ids[0], ids[1], ids[2], ids[3], correction);
	api.run(function(res) {
		var elem = document.getElementById(inputid);
		elem.value = res.cor;
		PCW.setupCorrectedInputField(elem, res);
	});
};

PCW.correctLine = function(anchor) {
	const ids = PCW.getIds(anchor);
	const correction = document.getElementById(anchor).value;
	var api = Object.create(PCW.Api);
	api.sid = PCW.getSid();
	api.setupForCorrectLine(ids[0], ids[1], ids[2], correction);
	api.run(function(res) {
		var elem = document.getElementById(anchor);
		PCW.setupCorrectedInputField(elem, res);
	});
};

PCW.correctAllLines = function(sid) {
	const regex = /(\d+)-(\d+)-(\d+)/;
	// iterate over all input nodes
	var items = document.getElementsByTagName("input");
	var ids = [];
	for (var i = 0; i < items.length; i++) {
		if (regex.test(items[i].id)) {
			ids = PCW.getIds(items[i].id);
			PCW.correctLine(items[i].id);
		}
	}
};

PCW.getSelectedWordFromInputElement = function(elem) {
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

PCW.displayConcordance = function(anchor) {
	const pid = PCW.getIds(anchor)[0];
	const selection = PCW.getSelectedWordFromInputElement(
	    document.getElementById(anchor));
	if (selection !== null) {
		document.getElementById("concordance-search-label").value =
		    selection;
		var api = Object.create(PCW.Api);
		api.sid = PCW.getSid();
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
			    "href", "conordance.php?pid=" + pid + "&q=" +
				encodeURI(res.query));
		});
	}
};
