// ================
// entities/docs.js
// ================

define(["app"], function(IPS_App){

  var Entities={};

Entities.API = {


  getJson: function(data){
    var defer = jQuery.Deferred();
        $.ajax({
        headers: { 
        'Accept': 'application/json',
        'Content-Type': 'application/json' 
         },
        url: "assets/js/api.json",
        type: "GET",
        dataType:"json",
        success: function(data) {
          console.log(data)
          defer.resolve(data);

            },
            error: function(data){
              defer.resolve(undefined);
            }
    });


    return defer.promise();
    
},

  




};



return Entities;

});
