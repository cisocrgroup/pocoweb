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
          data['request_url'] = this.url;
          defer.resolve(data);

            },
            error: function(data){
              data['request_url'] = this.url;
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
           data['request_url'] = this.url;
           defer.resolve(data);
         },
         error: function(data){
           data['request_url'] = this.url;
           defer.reject(data);
         }
    });
    return defer.promise();
  },


  loginCheck: function(){


     var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json',
           'Content-Type': 'application/json'
         },
         url: "rest/login?auth=" + App.getAuthToken(),
         type: "GET",
         success: function(data) {
          console.log(data);
           let user = App.getPcw().user;
             user['request_url'] = this.url;
             defer.resolve(user);
         },
         error: function(data){
           App.logout();
           let user = App.getPcw().user;
           user['request_url'] = this.url;
           defer.resolve(user);
         }
    });
    return defer.promise();

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
           var resp_data = {}
           resp_data['request_url'] = this.url;
           defer.resolve(resp_data);
         },
         error: function(data){
           var resp_data = {}
           resp_data['request_url'] = this.url;
           defer.reject(resp_data);
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
              data['request_url'] = this.url;
              defer.resolve(data);
            },
            error: function(data){
              data['request_url'] = this.url;
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
           data['request_url'] = this.url;
           defer.resolve(data);
         },
         error: function(data){
           data['request_url'] = this.url;
           defer.reject(data);
         }
    });

    return defer.promise();
  },
  updateUser: function(data){
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json',
           'Content-Type': 'application/json'
         },
         url: "rest/users/" + App.getCurrentUser().id + "?auth=" + App.getAuthToken(),
         type: "PUT",
         data: JSON.stringify({
           user: {
             name: data.name,
             email: data.email,
             institute: data.institute,
             id: data.id
           },
           password: data['new_password']
         }),
         success: function(data) {
           data['request_url'] = this.url;
           defer.resolve(data);
         },
         error: function(data){
           data['request_url'] = this.url;
           defer.reject(data);
         }
    });
    return defer.promise();
  },
   deleteUser: function(data){
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json',
           'Content-Type': 'application/json'
         },
        url: "rest/users/" + data.id + "?auth=" + App.getAuthToken(),
        type: "DELETE",
        success: function(data) {
              var resp_data = {}
              resp_data['request_url'] = this.url;
              defer.resolve(resp_data);
            },
            error: function(data){
              var resp_data = {}
              resp_data['request_url'] = this.url;
              defer.reject(resp_data);
            }
    });
    return defer.promise();
  },
   createUser: function(data){
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json',
           'Content-Type': 'application/json'
         },
        url: "rest/users?auth=" + App.getAuthToken(),
        type: "POST",
        data:JSON.stringify({
          user: {
            name: data.name,
            email: data.email,
            institute: data.institute,
            admin: data.admin
          },
          password: data.password
          }),
        success: function(data) {
              data['request_url'] = this.url;
              defer.resolve(data);
            },
            error: function(data){
              data['request_url'] = this.url;
              defer.reject(data);
            }
    });
    return defer.promise();
  }
};



return Entities;

});
