// ==============
// common/util.js
// ==============

define({


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
  //   input.firstChild.selectionStart = b;
  //   input.firstChild.selectionEnd = e;
  //   input.firstChild.focus();
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
}

});
