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
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json'
         },
         url: "rest/api-version",
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
    getLanguages: function(){
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json'
         },
         url: "rest/profile/languages",
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
  getDocumentation: function(){
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'text/html'
         },
         url: "doc.html",
         type: "GET",
         success: function(data) {
              defer.resolve(data);
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
