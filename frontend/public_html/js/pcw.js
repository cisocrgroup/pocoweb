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
		if (fmt[i] == '%') {
			if ((i + 1) < fmt.length) {
				i++;
				if (fmt[i] == '%') {
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
		if (http.readyState == 4 && http.status == 200) {
			callback(JSON.parse(http.responseText));
		}
	};
	var url =
	    sprintf(config.backend.url + config.backend.routes.search, pid, q);
	// console.log("url: " + url);
	http.open("GET", url, true);
	http.setRequestHeader("Authorization", sid);
	http.send(null);
}

function getSelectedWordFromInputElement(elem) {
	if (elem !== null && elem.tagName === "INPUT" && elem.type === "text") {
		var b = elem.selectionStart;
		var e = elem.selectionEnd;
		if ((e - b) > 0) {
			return elem.value.substring(b, e);
		}
	}
	return null;
}

function messageSelectWordFromInputElement(sid, pid, id) {
	var selection =
	    getSelectedWordFromInputElement(document.getElementById(id));
	if (selection !== null) {
		// console.log("selection: " + selection);
		document.getElementById("concordance-search").value = selection;
		getNumberOfConcordances(sid, pid, selection, function(res) {
			// console.log("res: " + res);
			var n = res.n;
			// console.log("n: " + n);
			var searchbutton = document.getElementById(
			    "concordance-search-button");
			if (n == 1) {
				searchbutton.innerHTML =
				    "Show " + n + " occurrence";
			} else {
				searchbutton.innerHTML =
				    "Show " + n + " occurrences";
			}
		});
	}
}
