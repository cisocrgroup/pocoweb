// ================
// entities/projects.js
// ================

define(["app"], function(App){
  var Entities={};

Entities.Page = Backbone.Model.extend({
     defaults:{
  bookId:null,
  box:null,
  imgFile:null,
  lines:[],
  nextPageId:0,
  ocrFile:"",
  prevPageId:0,
  projectId:""

     }
  });

Entities.Project = Backbone.Model.extend({
     urlRoot: "projects",
     defaults:{
  author:null,
  books:null,
  language:null,
  profilerUrl:"default",
  projectId:null,
  title:"",
  user:"",
  histPatterns:"",
  year:"2018"
     }
  });

Entities.API = {
  getProjects: function(){
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json'
         },
         url: "rest/books?auth=" + App.getAuthToken(),
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

  getPage: function(data){
    var defer = jQuery.Deferred();
    $.ajax({
      headers: {
        'Accept': 'application/json'
      },
      url: "rest/books/" + data.pid + "/pages/" + data.page + "?auth=" + App.getAuthToken(),
      type: "GET",
      success: function(data) {
        defer.resolve(new Entities.Page(data));
      },
      error: function(data){
        defer.reject(data);
      }
    });
    return defer.promise();
},

getProject: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
        headers: {
          'Accept': 'application/json'
        },
        url: "rest/books/" + data.pid + "?auth=" + App.getAuthToken(),
        type: "GET",
        success: function(data) {
          defer.resolve(new Entities.Project(data));
        },
        error: function(data){
          defer.reject(data);
        }
  });
  return defer.promise();
},

uploadProjectData: function(data){
    var defer = jQuery.Deferred();
  var auth = App.getAuthToken();
      $.ajax({
        url: "api/upload.php?auth="+auth,
        type: "POST",
        data: new FormData(data),
        cache:false,
        processData:false,
        contentType: false,
        success: function(data) {
              defer.resolve(data);
            },
            error: function(data){
              defer.reject(data);
            }
    });

    return defer.promise();
  },

downloadProject: function(data){
    data['backend_route'] = "download_project";
    var defer = jQuery.Deferred();
     $.ajax({

      url: "api/api_controller.php",
      type: "POST",
       data:data,
      success: function(data) {
        var result = new Entities.Project(JSON.parse(data));
        defer.resolve(result);

          },
          error: function(data){
            defer.reject(data);
          }
  });


  return defer.promise();

},


deleteProject: function(data){
    data['backend_route'] = "delete_project";
    console.log(data)
    var defer = jQuery.Deferred();
       $.ajax({

        url: "api/api_controller.php",
        type: "POST",
        data: data,
        success: function(data) {

              defer.resolve(data);
            },
            error: function(data){
              defer.reject(data);
            }
    });

    return defer.promise();
  },
  updateProject: function(data){
   data['projectdata']['year'] = parseInt(data['projectdata']['year']);
  console.log(data)
    var defer = jQuery.Deferred();
      $.ajax({
        headers: {
          'Accept': 'application/json'
        },
        url: "rest/books/" + data.pid + "?auth=" + App.getAuthToken(),
        type: "POST",
        data: JSON.stringify(data['projectdata']),
        success: function(data) {
              defer.resolve(data);
            },
            error: function(data){
              defer.reject(data);
            }
    });

    return defer.promise();
  },
   splitProject: function(data){
    data['backend_route'] = "split_project";
    console.log(data)
    var defer = jQuery.Deferred();
       $.ajax({

        url: "api/api_controller.php",
        type: "POST",
        data: data,
        success: function(data) {

              defer.resolve(JSON.parse(data));
            },
            error: function(data){
              defer.reject(data);
            }
    });

    return defer.promise();
  },

assignPackages: function(data){
  console.log(data);
    data['backend_route'] = "assign_packages";
    var defer = jQuery.Deferred();
     $.ajax({
      url: "api/api_controller.php",
      type: "POST",
       data:data,
      success: function(data) {
        defer.resolve(data);

          },
          error: function(data){
            defer.reject(data);
          }
  });


  return defer.promise();

},

    getLine: function(data){
    var defer = jQuery.Deferred();
       $.ajax({
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
        },
        url: "rest/books/"+data.pid+"/pages/"+data.page_id+"/lines/"+data.line_id+"?auth=" + App.getAuthToken(),
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
  correctLine: function(data){
    console.log(data)
    var defer = jQuery.Deferred();
       $.ajax({
       headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
        },
        url: "rest/books/"+data.pid+"/pages/"+data.page_id+"/lines/"+data.line_id+"?auth=" + App.getAuthToken(),
        type: "POST",
        data:JSON.stringify({correction:data.text}),
        success: function(data) {
              defer.resolve(data);
            },
            error: function(data){
              defer.reject(data);
            }
    });

    return defer.promise();
  },
    correctToken: function(data){
    console.log(data)
    var defer = jQuery.Deferred();
       $.ajax({
          headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
        },
        url: "rest/books/"+data.pid+"/pages/"+data.page_id+"/lines/"+data.line_id+"/tokens/"+data.token_id+"?auth=" + App.getAuthToken(),
        type: "POST",
        data:JSON.stringify({correction:data.token}),
        success: function(data) {
              defer.resolve(data);
            },
            error: function(data){
              defer.reject(data);
            }
    });

    return defer.promise();
  },
profileProject: function(data){
    console.log(data);
    var defer = jQuery.Deferred();
     $.ajax({
      headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/profile/books/"+data.pid+"?auth=" + App.getAuthToken(),
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

searchToken: function(data){
    var defer = jQuery.Deferred();
      $.ajax({
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
        },
        url: "rest/books/" + data.pid + "/search?auth=" + App.getAuthToken()+"&q="+data.q+"&p="+data.isErrorPattern+"&skip="+data.skip+"&max="+data.max,
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

getCorrectionSuggestions: function(data){
    data['backend_route'] = "get_correction_suggestions";
    console.log(data);
    var defer = jQuery.Deferred();
      $.ajax({
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
        },
        url: "rest/profile/books/" + data.pid + "?auth=" + App.getAuthToken()+"&q="+data.q,
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


getSuspiciousWords: function(data){

  var defer = jQuery.Deferred();
      $.ajax({
      headers: {
        'Accept': 'application/json'
      },
      url: "rest/profile/suspicious/books/" + data.pid + "?auth=" + App.getAuthToken(),
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
getErrorPatterns: function(data){

  var defer = jQuery.Deferred();
     $.ajax({
      headers: {
        'Accept': 'application/json'
      },
      url: "rest/profile/patterns/books/" + data.pid + "?auth=" + App.getAuthToken()+"&ocr=1",
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

getLexiconExtension: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
      headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/postcorrect/el/books/"+data.pid+"?auth=" + App.getAuthToken(),
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

startLexiconExtension: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
         headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/postcorrect/el/books/"+data.pid+"?auth=" + App.getAuthToken(),
      type: "POST",
       data:data,
      success: function(data) {
        defer.resolve(data);
          },
          error: function(data){
            defer.reject(data);
          }
  });


  return defer.promise();

},
getProtocol: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
     headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/postcorrect/rrdm/books/"+data.pid+"?auth=" + App.getAuthToken(),
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
startPostcorrection: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
     headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/postcorrect/rrdm/books/"+data.pid+"?auth=" + App.getAuthToken(),
      type: "POST",
      data:data,
      success: function(data) {
        defer.resolve(data);

          },
          error: function(data){
            defer.reject(data);
          }
  });


  return defer.promise();

},

getJobs: function(data){

  var defer = jQuery.Deferred();
    $.ajax({
      headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/jobs/"+data.pid+"?auth=" + App.getAuthToken(),
      type:"GET",
      success: function(data) {
        defer.resolve(data);
          },
          error: function(data){
            defer.reject(data);
          }
  });


  return defer.promise();

},
getCharmap: function(data){

	data['filter'] = "abcdefghijklmnopqrstuvwxyz" +
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ" +
		"0123456789" +
		"'\":,-+/@#$.;<>(){}[]\\&?!=*^~_";
  var defer = jQuery.Deferred();
      $.ajax({
      headers: {
        'Accept': 'application/json'
      },
      url: "rest/books/" + data.pid + "/charmap?auth=" + App.getAuthToken()+"&filter="+data['filter'],
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
