// ================
// entities/users.js
// ================

define(["app"], function(App){

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

  
  login: function(data){

    data['backend_route'] = "login";
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

  
  loginCheck: function(){
    var data= {};
    data['backend_route'] = "login_check";
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
    logout: function(data){
    var data= {};
    data['backend_route'] = "logout";
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

};



return Entities;

});
