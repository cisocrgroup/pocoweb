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

function sprintf(fmt) {
	var res = "";
	var j = 1;
	for (i = 0; i < fmt.length; i++) {
		if (fmt[i] === '%') {
			if ((i + 1) < fmt.length) {
				i++;
				if (fmt[i] === '%') {
					res += '%';
				} else {
					res += arguments[j++];
				}
			} else {
				res += fmt[i];
			}
		} else {
			res += fmt[i];
		}
	}
	return res;
}

function getNumberOfConcordances(sid, pid, q, callback) {
	var http = new XMLHttpRequest();
	http.onreadystatechange = function() {
		if (http.readyState === 4 && http.status === 200) {
			callback(JSON.parse(http.responseText));
		} else if (http.readyState === 4 && http.status !== 200) {
			console.error(
			    "backend returned status: " + http.status);
		}
	};
	var url =
	    sprintf(config.backend.url + config.backend.routes.search, pid, q);
	console.log(
	    "requesting concordances from url: " + url + " [" + sid + "]");
	http.open("GET", url, true);
	http.setRequestHeader("Authorization", sid);
	http.send(null);
}

function getSelectedWordFromInputElement(elem) {
	if (elem !== null && elem.tagName === "INPUT" && elem.type === "text") {
		var b = elem.selectionStart;
		var e = elem.selectionEnd;
		const regex = XRegExp('^\\PL*(.*?)\\PL*$');
		if ((e - b) > 0) {
			var m = regex.exec(elem.value.substring(b, e));
			return m[1];
		}
	}
	return null;
}

function messageSelectWordFromInputElement(sid, anchor) {
	var ids = getIds(anchor);
	var selection =
	    getSelectedWordFromInputElement(document.getElementById(anchor));
	if (selection !== null) {
		document.getElementById("concordance-search-label").value =
		    selection;
		getNumberOfConcordances(sid, ids[0], selection, function(res) {
			var searchbutton =
			    document.getElementById("concordance-search-label");
			setupConcordanceSearchButton(ids[0], searchbutton, res);
		});
	}
}

function setupConcordanceSearchButton(pid, button, obj) {
	var n = obj.nWords;
	var occurrences = "occurrences";
	if (n === 1) {
		occurrences = "occurrence";
	}
	button.innerHTML = "Show concordance of '" + obj.query + "' (" + n +
	    " " + occurrences + ")";
	button.parentNode.setAttribute(
	    "href",
	    "concordance.php?pid=" + pid + "&q=" + encodeURI(obj.query));
}

function getIds(anchor) {
	return anchor.split('-');
}

function correctLine(sid, anchor) {
	var correction = document.getElementById(anchor).value;
	var ids = getIds(anchor);
	sendCorrectionToServer(
	    sid, ids[0], ids[1], ids[2], correction, function(res) {
		    var elem = document.getElementById(anchor);
		    elem.value = res.cor;
		    elem.className += " corrected-line";
	    });
}

function sendCorrectionToServer(sid, pid, p, lid, correction, callback) {
	var http = new XMLHttpRequest();
	http.onreadystatechange = function() {
		if (http.readyState === 4 && http.status === 200) {
			callback(JSON.parse(http.responseText));
		} else if (http.readyState === 4 && http.status !== 200) {
			console.error(
			    "backend returned status: " + http.status);
		}
	};
	var url = sprintf(
	    config.backend.url + config.backend.routes["correct_line"], pid, p,
	    lid);
	console.log("sending correction to url: " + url + " [" + sid + "]");
	http.open("POST", url, true);
	http.setRequestHeader("Authorization", sid);
	http.setRequestHeader(
	    "Content-type", "application/json; charset=UTF-8");
	http.send(JSON.stringify({"d": correction}));
}

var PCW = {};
PCW.correctAllLines = function(sid) {
	console.log("correctAllLines [" + sid + "]");
	const regex = /(\d+)-(\d+)-(\d+)/;
	// iterate over all input nodes
	var items = document.getElementsByTagName("input");
	var ids = [];
	for (var i = 0; i < items.length; i++) {
		if (regex.test(items[i].id)) {
			ids = getIds(items[i].id);
			sendCorrectionToServer(
			    sid, ids[0], ids[1], ids[2], items[i].value,
			    function(res) {
				    var elem = document.getElementById(
					res.projectId + "-" + res.pageId + "-" +
					res.lineId);
				    elem.value = res.cor;
				    elem.classNmae += " corrected-line";
			    });
		}
	}
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
