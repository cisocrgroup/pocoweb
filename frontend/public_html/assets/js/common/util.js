// ==============
// common/util.js
// ==============

define({
	formatSuggestion: function(s) {
		var info = [];
        var pats = s.ocrPatterns.filter(function(pattern){
                     return pattern != "";
        });
		if (pats.length > 0) {
			info.push('ocr: ' + pats.join(','));
		}
        pats = s.histPatterns.filter(function(pattern){
                     return pattern != "";
        });
		if (pats.length > 0) {
			info.push('hist: ' + pats.join(','));
		}
		info.push('dist: ' + s.distance);
		info.push('weight: ' + s.weight.toFixed(2));
		return s.suggestion + ' (' + info.join(', ') + ')';
	},

replace_all : function(string,search, replacement) {
    return string.replace(new RegExp(search, 'g'), replacement);
},

escapeAsJSON: function(text) {
  replace_all = function(string, search, replacement) {
    return string.replace(new RegExp(search, 'g'), replacement);
  };
  text = replace_all(text, "\"", "\\\"");
  return JSON.parse("\"" + text + "\"");
},

 getBase64: function(file,callback) {
   var reader = new FileReader();
   reader.readAsDataURL(file);
   reader.onload = function () {

     data = reader.result.split(",")
     callback(data[1]);
   };
   reader.onerror = function (error) {
      callback(error);
  ;
   };
},

capitalizeFirstLetter:function(string) {
  return string.charAt(0).toUpperCase() + string.slice(1);
},

replaceSelectedText:function(replacementText) {
    var sel, range;
    if (window.getSelection) {
        sel = window.getSelection();
        if (sel.rangeCount) {
            range = sel.getRangeAt(0);
            range.deleteContents();

            range.insertNode(document.createTextNode(replacementText));

        }
    } else if (document.selection && document.selection.createRange) {
        range = document.selection.createRange();
        range.text = replacementText;
    }
},
toggleFromInputToText : function(anchor) {
  //pcw.log('pcw.toggleFromInputToText(' + anchor + ')');

  var input = document.getElementById('line-input-' + anchor);
  var text = document.getElementById('line-text-' + anchor);
  if (input === null || text === null) {
    return;
  }

  text.firstChild.data = $('#'+anchor).val();
  this.toggleBetweenTextAndInput(input, text);
},

toggleFromTextToInput : function(anchor) {
 // pcw.log('pcw.toggleFromTextToInput(' + anchor + ')');
  var input = document.getElementById('line-input-' + anchor);
  var text = document.getElementById('line-text-' + anchor);
  var b = 0;
  var e = 0;
  if (window.getSelection) {
    b = window.getSelection().anchorOffset;
    e = b + window.getSelection().toString().length;
  }
  this.toggleBetweenTextAndInput(text, input);
  // if (input.firstChild !== null) {
    input.firstChild.selectionStart = b;
    input.firstChild.selectionEnd = e;
    input.firstChild.focus();
  // }
  input.onkeyup = function(event) {
    if (event.keyCode === 13) { // <enter>
      pcw.correctLine(anchor);
      pcw.toggleFromInputToText(anchor);
    }
  };
},
toggleBetweenTextAndInput : function(hide, unhide) {
  if (hide === null || unhide === null) {
    return;
  }
  hide.setAttribute("hidden", "");
  unhide.removeAttribute("hidden");
},
getIds : function(anchor) {
  var ids = anchor.split('-');
  for (var i = 0; i < ids.length; i++) {
    ids[i] = parseInt(ids[i]);
  }
  return ids;
},

addAlignedLine : function(line,confidenceThreshold){


     var linetokens = line.tokens;


            var anchor = line["projectId"]+"-"+line["pageId"]+"-"+line['lineId'];

            var img_id = "line-img-"+anchor;
            var line_img = document.getElementById(img_id);
            var line_text =  $('#line-'+anchor);
	       		// line_text.find('.line-tokens').css('width', (Number(line_img.width)+50).toString() +'px'); ?? not needed to determine max length??
            var scalefactor = line_img.width / line.box.width;

             if(linetokens==undefined){
              var line_parent =  $('#line-anchor-'+anchor).parent();
              line_parent.empty();
              line_parent.append('<div class="alert alert-danger" role="alert"> Line ' +line['lineId']+' could not be displayed </div>');
              return;
              }

              let addDiv = function(text, title, width, box, offset, klass) {
                  let textdiv = $('<div>' + text + "</div>");
 				      let boxstr = "(" + box.left + "," + box.top + "," +
				      box.right + "," + box.bottom + ")";
                  textdiv.css('width', width * scalefactor);
                  textdiv.attr('boundingbox', boxstr);
                  textdiv.attr('title', title);
                  let div = $('<div class="tokendiv noselect"></div>').append(textdiv);
                  if (klass !== "") {
                    div.addClass(klass);
                  }
                  line_text.find('.line-tokens').append(div);
              };
              for(var i=0;i<linetokens.length;i++) {
                let token = linetokens[i];
                // add whitespace between tokens
                if (i > 0) {
                  let prev = linetokens[i-1];
                  let width = token.box.left - prev.box.right;
                  let box = {
                      left: prev.box.right+1,
                      right: token.box.left-1,
                      top: token.box.top,
                      bottom: token.box.bottom
                  };
                  let offset = prev.offset + prev.cor.trim().length + 1;
                  addDiv("","", width, box, offset,"");
                }
                let corrected = "";
                if (!line.isManuallyCorrected) {
                  corrected = token.isManuallyCorrected;
                  if (token.isManuallyCorrected) {
                    corrected = "manually_corrected token-text";
                  } else if (token.isAutomaticallyCorrected) {
                    corrected = "automatically_corrected token-text";
                  }
                  else if(token.averageConfidence.toFixed(2)&&token.averageConfidence.toFixed(2)<=confidenceThreshold){
                    corrected = "confidence_threshold token-text"
                  }
                }
                addDiv(token.cor.trim(),"average confidence: "+token.averageConfidence.toFixed(2), token.box.width, token.box, token.offset,corrected);
               }
},

addLine : function(line,confidenceThreshold){


     var linetokens = line.tokens;


            var anchor = line["projectId"]+"-"+line["pageId"]+"-"+line['lineId'];

            var img_id = "line-img-"+anchor;
            var line_img = document.getElementById(img_id);
            var line_text =  $('#line-'+anchor);
            // line_text.find('.line-tokens').css('width', (Number(line_img.width)+50).toString() +'px'); ?? not needed to determine max length??
            var scalefactor = line_img.width / line.box.width;

             if(linetokens==undefined){
              var line_parent =  $('#line-anchor-'+anchor).parent();
              line_parent.empty();
              line_parent.append('<div class="alert alert-danger" role="alert"> Line ' +line['lineId']+' could not be displayed </div>');
              return;
              }

              let addDiv = function(text,title, width, box, offset, klass) {
                  let textdiv = $('<div>' + text + "</div>");
              let boxstr = "(" + box.left + "," + box.top + "," +
              box.right + "," + box.bottom + ")";
                  // textdiv.css('width', width * scalefactor);
                  textdiv.attr('boundingbox', boxstr);
                  textdiv.attr('title', title);
                  let div = $('<div class="tokendiv noselect"></div>').append(textdiv);
                  if (klass !== "") {
                    div.addClass(klass);
                  }
                  line_text.find('.line-tokens').append(div);
              };
              for(var i=0;i<linetokens.length;i++) {
                let token = linetokens[i];
                // add whitespace between tokens
                if (i > 0) {
                  let prev = linetokens[i-1];
                  let width = token.box.left - prev.box.right;
                  let box = {
                      left: prev.box.right+1,
                      right: token.box.left-1,
                      top: token.box.top,
                      bottom: token.box.bottom
                  };
                  let offset = prev.offset + prev.cor.trim().length + 1;
                  addDiv("", width, box, offset,"");
                }
                let corrected = "";
                if (!line.isManuallyCorrected) {
                  corrected = token.isManuallyCorrected;
                  if (token.isManuallyCorrected) {
                    corrected = "manually_corrected token-text";
                  } else if (token.isAutomaticallyCorrected) {
                    corrected = "automatically_corrected token-text";
                  }
                  else if(token.averageConfidence.toFixed(2)&&token.averageConfidence.toFixed(2)<=confidenceThreshold){
                    corrected = "confidence_threshold token-text"
                  }
                }
                addDiv(token.cor.trim(),"average confidence: "+token.averageConfidence.toFixed(2), token.box.width, token.box, token.offset,corrected);
               }
},

  copyStringToClipboard :function(str) {
       // Create new element
       var el = document.createElement('textarea');
       // Set value (string to be copied)
       el.value = str;
       // Set non-editable to avoid focus and move outside of view
       el.setAttribute('readonly', '');
       el.style = {position: 'absolute', left: '-9999px'};
       document.body.appendChild(el);
       // Select text inside element
       el.select();
       // Copy text to clipboard
       document.execCommand('copy');
       // Remove temporary element
       document.body.removeChild(el);
    },

    defaultErrorHandling: function(response,mode) {
      
      require(["app"],function(App){
        if(response.responseJSON!=undefined){
           if(response.status==401){
             var mainRegion = App.mainLayout.getRegion('mainRegion');
             mainRegion.empty();

             // re-render navbar on every 401

             App.logout(); // automatically logout user when 401?

             setTimeout(function() {

             var headerRegion = App.mainLayout.getRegion('headerRegion');
             headerRegion.currentView.getRegion('navbarRegion').currentView.options.user={id:-1}; 
             headerRegion.currentView.getRegion('navbarRegion').currentView.render(); 

             }, 50);

         
             
             // display login screen
             App.trigger("nav:login",false);
         }
                 console.log(response.request_url);

         App.mainmsg.updateContent("Error "+ response.responseJSON.code +" ("+response.responseJSON.status+") "+response.responseJSON.message,mode,true,response.request_url);
         }
         else if(response.responseText!=undefined){
         App.mainmsg.updateContent("Error "+ response.status+" ("+response.statusText+") ",mode,true,response.request_url);

         }
         else if(response.message!=undefined){
         App.mainmsg.updateContent("Error "+ response.code+" ("+response.message+") ",mode,true,response.request_url);
         }
         else{
            App.mainmsg.updateContent(response,mode,true,response.request_url);
        }
      });


    },

    removeStatusClasses:function(elem){
      elem.removeClass('disabled');
      elem.removeClass('green');
      elem.removeClass('yellow');
      elem.removeClass('red');
    },

    fit_to_screen:function(img) {

    img.removeAttribute("style");
    var winX = window.innerWidth -15 + "px";
    var winY = window.innerHeight-15 + "px";
    var vbar = false;
    if (document.body.scrollHeight > document.body.clientHeight) // vertical scrollbar
    {
            img.style.height = winY;
            vbar = true;
    }
    if (document.body.scrollWidth > document.body.clientWidth) // horizontal scrollbar
    {
            if (vbar) // both scrollbars
            {
                    if ((document.body.scrollHeight - document.body.clientHeight) > (document.body.scrollWidth - document.body.clientWidth)) // let's see which one is bigger
                    {
                            img.removeAttribute("style");
                            img.style.height = winY;
                    }
                    else
                    {
                            img.removeAttribute("style");
                            img.style.width = winX;
                    }
            }
            else
            {
                    img.removeAttribute("style");
                    img.style.width = winX;
            }
    }
},

addPostCorrectionClassification: function(data) {
  // add never, always and sometimes
  let types = {}; // {'normalized': [vals...]}
  for (let key in data.corrections) {
    let val = data.corrections[key];
    let norm = val.normalized;
    if (!(norm in types)) {
      types[norm] = [];
    }
    types[norm].push(val);
  }
  // split types into sometimes, always and never
  let classify = function(vals) {
                    let all = true;
                    let none = true;
                    vals.forEach(function(val){
                      if (val.taken) {
                        none = false;
                      } else {
                        all = false;
                      }
                    });
                    if (all) {
                      return 'always';
                    }
                    if (none) {
                      return 'never';
                    }
                    return 'sometimes';
                  };
  data.never = {};
  data.always = {};
  data.sometimes = {};
  for (let key in types) {
    let klass = classify(types[key]);
    data[klass][key] = types[key];
  }
  return data;
},

getBreadcrumbs(breadcrumbs){
  var result = "";
  var d = "";
  for (var i=0;i<breadcrumbs.length;i++){
    result+= d
    if(breadcrumbs[i]['url']!=""){
      result += "<a href='"+breadcrumbs[i]['url']+"'>";
    }

    result+="<span>"+breadcrumbs[i]['title']+"</span>";

     if(breadcrumbs[i]['url']!=""){
         result += "</a>";
     }

    d = " / ";
  }
  return result;
}
});
