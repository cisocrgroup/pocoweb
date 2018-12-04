// ================
// entities/projects.js
// ================

define(["app"], function(IPS_App){

  var Entities={};

Entities.Project = Backbone.Model.extend({
     urlRoot: "api/v1/proposals",

     defaults:{
  author:null,
  books:null,
  language:null,
  profilerUrl:null,
  projectId:null,
  title:"",
  user:"",
  year:""
  
     }
  });


Entities.API = {


  getProjects: function(){
    var defer = jQuery.Deferred();
        $.ajax({
        
        url: "/api/projects/list",
        type: "GET",
        dataType:"json",
        success: function(data) {
          defer.resolve(data);

            },
            error: function(data){
              defer.resolve(undefined);
            }
    });


    return defer.promise();
    
},

getProject: function(id){
  var defer = jQuery.Deferred();
      $.ajax({
      
      url: "/api/projects/"+id,
      type: "GET",
      dataType:"json",
      success: function(data) {
        var result = new Entities.Project(data)
        defer.resolve(result);

          },
          error: function(data){
            defer.resolve(undefined);
          }
  });


  return defer.promise();
  
},

createProject: function(data){
    var defer = jQuery.Deferred();
       $.ajax({
        headers: {
        'Accept': 'application/json',
        'Content-Type': 'application/json'
         },
        url: "api/projects/create",
        type: "POST",
        data:JSON.stringify(data),
        dataType: "json",
        success: function(data) {

              defer.resolve(data);
            },
            error: function(data){
              defer.resolve(undefined);
            }
    });

    return defer.promise();
  },

updateProject: function(id,data){
    var defer = jQuery.Deferred();
       $.ajax({
        headers: {
        'Accept': 'application/json',
        'Content-Type': 'application/json'
         },
        url: "api/projects/"+id+"/update",
        type: "PUT",
        data:JSON.stringify(data),
        dataType: "json",
        success: function(data) {

              defer.resolve(data);
            },
            error: function(data){
              defer.resolve(undefined);
            }
    });

    return defer.promise();
  },

deleteProject: function(id){
    var defer = jQuery.Deferred();
       $.ajax({
        headers: {
        'Accept': 'application/json',
        'Content-Type': 'application/json'
         },
        url: "api/projects/"+id+"/delete",
        type: "DELETE",
        dataType: "json",
        success: function(data) {

              defer.resolve(data);
            },
            error: function(data){
              defer.resolve(undefined);
            }
    });

    return defer.promise();
  },

addBook: function(id,data){
    var defer = jQuery.Deferred();
       $.ajax({
        headers: {
        'Accept': 'application/json',
        'Content-Type': 'application/json'
         },
        url: "api/projects/"+id+"/add-book",
        type: "POST",
        data:JSON.stringify(data),
        dataType: "json",
        success: function(data) {

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
