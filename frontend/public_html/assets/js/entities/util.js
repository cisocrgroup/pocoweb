// ================
// entities/util.js
// ================

define(["app"], function(App){

  var Entities={};

 

Entities.API = {


  startTraining: function(data){
    var defer = jQuery.Deferred();
    $.ajax({
         headers: { 
        'Accept': 'application/json',
        'Content-Type': 'application/json' 
        },
        url: "/api/trainclassifier",
        type: "POST",
        data:JSON.stringify(data),
        success: function(data) {

             defer.resolve(data);

            },
            error: function(data){
              defer.resolve(undefined);
            }
    });

    return defer.promise();
    
},
  getApiVersion: function(){
    var data = {};
    data['backend_route'] = "api_version";
    var defer = jQuery.Deferred();
       $.ajax({
     
        url: "api/api_controller.php",
        type: "POST",
        data:data,
        success: function(data) {

              defer.resolve(JSON.parse(data));
            },
            error: function(data){
              defer.reject(data);
            }
    });

    return defer.promise();
  },
    getLanguages: function(){
    var data = {};
    data['backend_route'] = "languages";
    var defer = jQuery.Deferred();
       $.ajax({
     
        url: "api/api_controller.php",
        type: "POST",
        data:data,
        success: function(data) {

              defer.resolve(JSON.parse(data));
            },
            error: function(data){
              defer.reject(data);
            }
    });

    return defer.promise();
  }






};



return Entities;

});
