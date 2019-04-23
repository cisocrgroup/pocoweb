// ==============
// common/util.js
// ==============

define({


replace_all : function(string,search, replacement) {
    return string.replace(new RegExp(search, 'g'), replacement);
},

get_correction_class: function(obj) {
  if (obj["isFullyCorrected"]) {
    return " fully-corrected";
  } else if (obj["isPartiallyCorrected"]) {
    return " partially-corrected";
  }
  return "";
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

addAlignedLine : function(line){
     var linetokens = line.tokens;
            var anchor = line["projectId"]+"-"+line["pageId"]+"-"+line['lineId'];

            var img_id = "line-img-"+anchor;
            var line_img = document.getElementById(img_id);
            var line_text =  $('#line-'+anchor);
			line_text.find('.line-tokens').css('width', (Number(line_img.width)+50).toString() +'px');
            var scalefactor = line_img.width / line.box.width;

              for(var i=0;i<linetokens.length;i++) {

                var token = linetokens[i];
                var cordiv;
                if(token.cor.includes(" ")){
                   cordiv = $('<div>'+token.cor+"</div>");
                }
                else {
                   cordiv = $('<div>'+token.cor.trim()+"</div>");
                }

                 var div = $('<div class="tokendiv noselect"></div>').append(cordiv);
                line_text.find('.line-tokens').append(div);
                var box = token['box'];

                    var div_length = token.box.width*scalefactor ;
                    cordiv.css('width',div_length);

               }
}

});
