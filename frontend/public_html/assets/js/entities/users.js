// ================
// entities/users.js
// ================

define(["app"], function(App){

  var Entities={};

Entities.User = Backbone.Model.extend({
  defaults:{
  name:"",
  email:"",
  institute:"",
  id:null,
  admin:"",
  password:"",
  new_password:""

     }
  });

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
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json',
           'Content-Type': 'application/json'
         },
         url: "rest/login",
         type: "POST",
         data: JSON.stringify(data),
         success: function(data) {
           defer.resolve(data);
         },
         error: function(data){
           defer.reject(data);
         }
    });
    return defer.promise();
  },


  loginCheck: function(){
    if (sessionStorage.getItem('pcw')) {
      return App.getCurrentUser();
    } else {
      return -1;
    }
  },
    logout: function(data){
      var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json'
         },
         url: "rest/logout?auth=" + App.getAuthToken(),
         type: "GET",
         success: function(data) {
           sessionStorage.removeItem('pcw');
           defer.resolve(data);
         },
         error: function(data){
           sessionStorage.removeItem('pcw');
           defer.reject(data);
         }
    });
    return defer.promise();
  },

    getUsers: function(){
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json'
         },
         url: "rest/users?auth=" + App.getAuthToken(),
         type: "GET",
         success: function(data) {
              defer.resolve(data);
            },
            error: function(data){
              defer.reject(data);
            }
    });
    return defer.promise();
  },
    getUser: function(){
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json'
         },
         url: "rest/users/" + App.getCurrentUser().id + "?auth=" + App.getAuthToken(),
         type: "GET",
         success: function(data) {
           defer.resolve(data);
         },
         error: function(data){
           defer.reject(data);
         }
    });

    return defer.promise();
  },
  updateUser: function(data){
    data['backend_route'] = "update_user";
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json',
           'Content-Type': 'application/json'
         },
         url: "rest/users/" + App.getCurrentUser().id + "?auth=" + App.getAuthToken(),
         type: "PUT",
         data: JSON.stringify(data),
         success: function(data) {
           defer.resolve(data);
         },
         error: function(data){
           defer.reject(data);
         }
    });
    return defer.promise();
  },
      deleteUser: function(data){
    data['backend_route'] = "delete_user";
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

   createUser: function(data){
    data['backend_route'] = "create_user";
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
