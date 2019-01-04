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
}


});
