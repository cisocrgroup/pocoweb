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

pcw.log = function(msg) {
	if (pcw.config.frontend.debug) {
		console.log("(pcw) " + msg);
	}
};

pcw.onLoad = function() {
	pcw.setApiVersion();
};

pcw.onLoadWithPid = function(pid) {
	pcw.setApiVersion();
	pcw.setErrorDropdowns(pid);
	pcw.setCorrectionSuggestions(pid);
	pcw.markSuspiciousWords(pid);
};

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
	var regex = /concordance-token-input-(\d+-\d+-\d+-\d+)/;
	if (suggestion !== null) {
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
	var name = "pcw-sid=";
	var cookies = document.cookie.split(';');
	for (var i = 0; i < cookies.length; i++) {
		var c = cookies[i];
		while (c.charAt(0) === ' ') {
			c = c.substring(1, c.length);
		}
		if (c.indexOf(name) === 0) {
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
	api.run(function(status, res) {
		var elem = document.getElementById('pcw-api-version');
		if (elem !== null) {
			elem.innerHTML = 'Api-Version: ' + res.version;
		}
	});
};

pcw.setLoggedInUserImpl = function(status, user) {
	pcw.log("status: " + status);
	pcw.log("user: " + JSON.stringify(user));
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

pcw.setCorrectionStatus = function(elem, fully, partial) {
	if (elem === null) {
		return;
	}
	elem.classList.remove('fully-corrected');
	elem.classList.remove('partially-corrected');
	if (fully) {
		elem.classList.add('fully-corrected');
	} else if (partial) {
		elem.classList.add('partially-corrected');
	}
};

pcw.correctWord = function(anchor) {
	var ids = pcw.getIds(anchor);
	var inputid = "concordance-token-input-" + anchor;
	var correction = document.getElementById(inputid).value;
	var api = Object.create(pcw.Api);
	api.sid = pcw.getSid();
	api.setupForCorrectWord(ids[0], ids[1], ids[2], ids[3], correction);
	api.run(function(status, res) {
		var elem = document.getElementById(inputid);
		if (elem !== null) {
			elem.value = res.cor;
			pcw.setCorrectionStatus(
			    elem, res.isFullyCorrected,
			    res.isPartiallyCorrected);
		}
	});
};

pcw.onClickCorrectLineButton = function(anchor) {
	pcw.log('pcw.onClickCorrectLineButton(' + anchor + ')');
	pcw.correctLine(anchor);
	pcw.toggleFromInputToText(anchor);
};

pcw.correctLine = function(anchor) {
	pcw.log('pcw.correctLine(' + anchor + ')');
	var ids = pcw.getIds(anchor);
	var correction = document.getElementById(anchor).value;
	var api = Object.create(pcw.Api);
	api.sid = pcw.getSid();
	api.setupForCorrectLine(ids[0], ids[1], ids[2], correction);
	api.run(function(status, res) {
		var fully = res.isFullyCorrected;
		var partial = res.isPartiallyCorrected;
		var input = document.getElementById(anchor);
		if (input !== null) {
			pcw.log("setting correction status input: " + anchor);
			input.value = res.cor;
			pcw.setCorrectionStatus(input, fully, partial);
		}
		var text = document.getElementById('line-text-' + anchor);
		if (text !== null) {
			pcw.log("setting correction status text: " + anchor);
			pcw.setCorrectionStatus(text, fully, partial);
			text.replaceChild(
			    document.createTextNode(res.cor),
			    text.childNodes[0]);
			var aapi = Object.create(pcw.Api);
			aapi.sid = pcw.getSid();
			aapi.setupForGetSuspiciousWords(ids[0], ids[1], ids[2]);
			aapi.run(pcw.markSuspiciousWordsInLine);
		}
	});
};

pcw.correctAllLines = function(sid) {
	var regex = /(\d+)-(\d+)-(\d+)/;
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
		var sel = {};
		sel.b = elem.selectionStart;
		sel.e = elem.selectionEnd;
		// we need unicode aware regexes to handle words like `ſunt` ...
		var regex = XRegExp('^(\\PL*)(.*?)(\\PL*)$');
		if ((sel.e - sel.b) > 0) {
			var m = regex.exec(elem.value.substring(sel.b, sel.e));
			sel.str = m[2];
			sel.b = sel.b + m[1].length;
			sel.e = sel.e - m[3].length;
			return sel;
		}
	}
	return null;
};

pcw.displayConcordance = function(anchor) {
	var ids = pcw.getIds(anchor);
	var pid = ids[0];
	var selection = pcw.getSelectedWordFromInputElement(
	    document.getElementById(anchor));
	if (selection !== null) {
		selection.ids = ids;
		pcw.log("selection: " + JSON.stringify(selection));
		pcw.setConcordanceSearchLabel(pid, selection);
		pcw.setSuggestionsDropdown(pid, selection);
	}
};

pcw.setConcordanceSearchLabel = function(pid, selection) {
	document.getElementById("concordance-search-label").value =
	    selection.str;
	var api = Object.create(pcw.Api);
	api.sid = pcw.getSid();
	api.setupForGetConcordance(pid, selection.str);
	api.run(function(status, res) {
		var searchButton =
		    document.getElementById('concordance-search-label');
		var n = res.nWords;
		var ocs = "occurrences";
		if (n === 1) {
			ocs = "occurrence";
		}
		searchButton.innerHTML = 'Show concordance of "' + res.query +
		    '" (' + n + ' ' + ocs + ')';
		searchButton.parentNode.setAttribute(
		    "href", "concordance.php?pid=" + pid + "&q=" +
			encodeURI(res.query));

	});
};

pcw.setSuggestionsDropdown = function(pid, selection) {
	pcw.log(
	    "getting suggestions for project #" + pid + " q=" + selection.str);
	// clear dropdown menu node for new items
	var dropdown = document.getElementById('pcw-suggestions-dropdown');
	while (dropdown.hasChildNodes()) {
		dropdown.removeChild(dropdown.lastChild);
	}
	var api = Object.create(pcw.Api);
	api.sid = pcw.getSid();
	api.setupForSearchSuggestions(pid, selection.str);
	api.run(function(status, res) {
		pcw.log("timestamp: " + pcw.timestampToISO8601(res.timestamp));
		suggs = res.suggestions || [];
		suggs.sort(function(a, b) { return b.weight - a.weight; });
		for (var i = 0; i < suggs.length; i++) {
			pcw.addSuggestionDropdownItem(
			    dropdown, suggs[i], selection);
		}
	});
};

pcw.addSuggestionDropdownItem = function(dropdown, s, selection) {
	pcw.log(
	    "suggestion: \"" + s.suggestion + "\", weight: " + s.weight +
	    ", distance: " + s.distance);
	var li = document.createElement("li");
	var a = document.createElement("a");
	var t = document.createTextNode(
	    s.suggestion + " (patts: " + s.patterns.join(',') + ", dist: " +
	    s.distance + ", weight: " + s.weight.toFixed(2) + ")");
	a.onclick = function() {
		pcw.setSuggestionToSelection(selection, s.suggestion);
	};
	a.appendChild(t);
	li.appendChild(a);
	dropdown.appendChild(li);
};

pcw.setSuggestionToSelection = function(selection, suggestion) {
	pcw.log(
	    "setSuggestionToSelection (" + suggestion + "): " +
	    JSON.stringify(selection));
	var input = document.getElementById(selection.ids.join('-'));
	var oldval = input.value;
	var newval = oldval.substring(0, selection.b) + suggestion +
	    oldval.substring(selection.e);
	input.value = newval;
};

pcw.orderProfile = function(pid) {
	pcw.log("ordering profile for project #" + pid);
	var api = Object.create(pcw.Api);
	api.sid = pcw.getSid();
	api.setupForOrderProfile(pid);
	api.run(function(status, res) {
		pcw.log("ordered profile for project #" + pid);
	});
};

pcw.setErrorDropdowns = function(pid) {
	var ddep = document.getElementById("pcw-error-patterns-dropdown");
	var ddet = document.getElementById("pcw-error-tokens-dropdown");
	if (ddep === null || ddet === null) {
		return;
	}
	var api = Object.create(pcw.Api);
	api.sid = pcw.getSid();
	api.setupForGetAllSuggestions(pid);
	api.run(function(status, res) {
		if (ddep !== null) {
			pcw.setErrorPatternsDropdown(pid, ddep, res);
		}
		if (ddet !== null) {
			pcw.setErrorTokensDropdown(pid, ddet, res);
		}
	});
};

pcw.setErrorPatternsDropdown = function(pid, dropdown, res) {
	pcw.log("setErrorPatternsDropdown");
	var patterns = {};
	var suggs = res.suggestions || [];
	for (var i = 0; i < suggs.length; i++) {
		var id = suggs[i].pageId + '-' + suggs[i].lineId + '-' +
		    suggs[i].tokenId;
		for (var j = 0; j < suggs[i].patterns.length; j++) {
			var pat = suggs[i].patterns[j].toLowerCase();
			var set = patterns[pat] || {};
			set[i] = true;
			patterns[pat] = set;
		}
	}
	var counts = [];
	for (var p in patterns) {
		counts.push(
		    {pattern: p, count: Object.keys(patterns[p]).length});
	}
	counts.sort(function(a, b) { return b.count - a.count; });
	var onclick = function(pid, c) {
		return function() {
			pcw.log(c.pattern + ": " + c.count);
			var pat = encodeURI(c.pattern);
			var href = "concordance.php?pid=" + pid + "&q=" + pat +
			    "&error-pattern";
			window.location.href = href;
		};
	};
	for (var ii = 0; ii < counts.length; ii++) {
		var c = counts[ii];
		var a = pcw.appendErrorCountItem(dropdown, c.pattern, c.count);
		a.onclick = onclick(pid, c);
	}
};

pcw.setErrorTokensDropdown = function(pid, dropdown, res) {
	pcw.log("setErrorsDropdown");
	var tokens = {};
	var suggs = res.suggestions || [];
	for (var i = 0; i < suggs.length; i++) {
		var id = suggs[i].pageId + '-' + suggs[i].lineId + '-' +
		    suggs[i].tokenId;
		var tok = suggs[i].token.toLowerCase();
		var set = tokens[tok] || {};
		set[id] = true;
		tokens[tok] = set;
	}
	var counts = [];
	for (var p in tokens) {
		counts.push({token: p, count: Object.keys(tokens[p]).length});
	}
	counts.sort(function(a, b) { return b.count - a.count; });
	var onclick = function(pid, c) {
		return function() {
			pcw.log(c.token + ": " + c.count);
			var pat = encodeURI(c.token);
			var href = "concordance.php?pid=" + pid + "&q=" + pat;
			window.location.href = href;
		};
	};
	for (var ii = 0; ii < counts.length; ii++) {
		c = counts[ii];
		var a = pcw.appendErrorCountItem(dropdown, c.token, c.count);
		a.onclick = onclick(pid, c);
	}
};

pcw.appendErrorCountItem = function(dropdown, item, count) {
	var li = document.createElement("li");
	var a = document.createElement("a");
	var t = document.createTextNode(item + ": " + count);
	a.appendChild(t);
	li.appendChild(a);
	dropdown.appendChild(li);
	return a;
};

pcw.setCorrectionSuggestions = function(pid) {
	var x = document.getElementsByTagName('button');
	for (var i = 0; i < x.length; i++) {
		if (/concordance-token-dropdown-/.test(x[i].id)) {
			var ids =
			    pcw.getIds(/\d+-\d+-\d+-\d+/.exec(x[i].id)[0]);
			pcw.doSetCorrectionSuggestions(
			    ids[0], ids[1], ids[2], ids[3]);
		}
	}
};

pcw.doSetCorrectionSuggestions = function(pid, p, lid, tid) {
	var api = Object.create(pcw.Api);
	api.sid = pcw.getSid();
	api.setupForGetSuggestions(pid, p, lid, tid);
	var anchor = pid + '-' + p + '-' + lid + '-' + tid;
	var input =
	    document.getElementById('concordance-token-input-' + anchor);
	var ul =
	    document.getElementById('concordance-token-suggestions-' + anchor);
	var checkbox =
	    document.getElementById('concordance-token-checkbox-' + anchor);
	var onclick = function(checkbox, input, s) {
		return function() {
			checkbox.checked = true;
			input.value = s.suggestion;
		};
	};
	api.run(function(status, res) {
		var suggs = res.suggestions || [];
		for (var i = 0; i < suggs.length; i++) {
			var a = pcw.addItemToSuggestionsMenu(ul, suggs[i]);
			a.onclick = onclick(checkbox, input, suggs[i]);
		}
	});
};

pcw.addItemToSuggestionsMenu = function(ul, s) {
	var li = document.createElement("li");
	var a = document.createElement("a");
	var t = document.createTextNode(
	    s.suggestion + " (patts: " + s.patterns.join(',') + ", dist: " +
	    s.distance + ", weight: " + s.weight.toFixed(2) + ")");
	a.appendChild(t);
	li.appendChild(a);
	ul.appendChild(li);
	return a;
};

pcw.clearNode = function(node) {
	while (node.firstChild) {
		node.removeChild(node.firstChild);
	}
};

pcw.setProfilerLanguages = function() {
	var input = document.getElementById('pcw-project-profiler');
	if (input === null ||
	    (input.value != 'local' && !input.value.startsWith('http'))) {
		return;
	}
	var langs = document.getElementById('pcw-project-languages');
	if (langs === null) {
		return;
	}
	pcw.clearNode(langs);
	var api = Object.create(pcw.Api);
	api.sid = pcw.getSid();
	api.setupForGetLanguages(input.value);
	api.run(function(s, res) {
		pcw.clearNode(langs);
		for (var i = 0; i < res.languages.length; i++) {
			t = document.createTextNode(res.languages[i]);
			o = document.createElement("option");
			o.appendChild(t);
			langs.appendChild(o);
		}
	});
};

pcw.toggleBetweenTextAndInput = function(hide, unhide) {
	if (hide === null || unhide === null) {
		return;
	}
	hide.classList.add('hidden');
	unhide.classList.remove('hidden');
};

pcw.toggleFromInputToText = function(anchor) {
	pcw.log('pcw.toggleFromInputToText(' + anchor + ')');
	var input = document.getElementById('line-input-' + anchor);
	var text = document.getElementById('line-text-' + anchor);
	if (input === null || text === null) {
		return;
	}
	text.firstChild.data = input.firstChild.value;
	pcw.toggleBetweenTextAndInput(input, text);
};

pcw.toggleFromTextToInput = function(anchor) {
	pcw.log('pcw.toggleFromTextToInput(' + anchor + ')');
	var input = document.getElementById('line-input-' + anchor);
	var text = document.getElementById('line-text-' + anchor);
	var b = 0;
	var e = 0;
	if (window.getSelection) {
		b = window.getSelection().anchorOffset;
		e = b + window.getSelection().toString().length;
	}
	pcw.toggleBetweenTextAndInput(text, input);
	if (input.firstChild !== null) {
		input.firstChild.selectionStart = b;
		input.firstChild.selectionEnd = e;
		input.firstChild.focus();
	}
	input.onkeyup = function(event) {
		if (event.keyCode === 13) { // <enter>
			pcw.correctLine(anchor);
			pcw.toggleFromInputToText(anchor);
		}
	};
};

pcw.markSuspiciousWords = function(pid) {
	var divs = document.getElementsByTagName("div");
	var p = 0;
	for (var i = 0; i < divs.length; i++) {
		var div = divs[i];
		if (/line-text-\d+-\d+\d+/.test(div.id)) {
			var ids = pcw.getIds(div.id.substring(10));
			var api = Object.create(pcw.Api);
			api.sid = pcw.getSid();
			api.setupForGetSuspiciousWords(pid, ids[1], ids[2]);
			api.run(pcw.markSuspiciousWordsInLine);
		}
	}
};

pcw.markSuspiciousWordsInLine = function(status, res) {
	if (res.suspiciousWords.length <= 0) {
		return;
	}
	var w0 = res.suspiciousWords[0];
	var id = res.projectId + '-' + w0.pageId + '-' + w0.lineId;
	var line = document.getElementById('line-text-' + id);
	if (line === null || line.firstChild === null) {
		return;
	}
	var text = line.firstChild.nodeValue;
	for (var i = 0; i < res.suspiciousWords.length; i++) {
		var wi = res.suspiciousWords[i].token;
		text = text.replace(
		    new RegExp(wi, 'g'),
		    "<span class=\"underline\">" + wi + "</span>");
	}
	line.innerHTML = text;
};

pcw.openImageWindow = function(img, l, t, w, h) {
	var ww = window.innerWidth || document.documentElement.clientWidth ||
	    document.body.clientWidth;

	var wh = window.innerHeight || document.documentElement.clientHeight ||
	    document.body.clientHeight;
	var nww = ww / 2;
	var nwh = wh;
	var doc = window.open(
	    img, "image", "width=" + nww + ",height=" + nwh + ",left=" + nww);
	pcw.log("box: l:" + l + ",t:" + t + ",w:" + w + ",h:" + h);
	doc.addEventListener("load", function() {
		// console.log("doc loaded");
		// var body = doc.document.getElementsByTagName("body")[0];
		// // var img = doc.document.getElementsByTagName('img')[0];
		// var canvas = doc.document.createElement('canvas');
		// canvas.setAttribute('id', 'canvas');
		// canvas.setAttribute('width', w);
		// canvas.setAttribute('height', h);
		// canvas.setAttribute(
		//     'style',
		//     'position:absolute; border:1px solid #000000; top:0px;
		//     left:0px; z-index:1;');
		// body.appendChild(canvas);
		// console.log("callback done");
	}, true);
	doc.focus();
};

pcw.timestampToISO8601 = function(ts) {
	return new Date(ts * 1000).toUTCString();
};
