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
  data.auth = App.getAuthToken();
      $.ajax({
        url: "api/upload.php",
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

  createProject: function(data){
    data['backend_route'] = "create_project";
    console.log(data)
    var defer = jQuery.Deferred();
       $.ajax({
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
        },
         url: "rest/books/" + data.pid + "?auth=" + App.getAuthToken(),
        type: "GET",

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
    data['backend_route'] = "update_project";
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
    data['backend_route'] = "get_line";
    console.log(data)
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
  correctLine: function(data){
    data['backend_route'] = "correct_line";
    console.log(data)
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
    correctToken: function(data){
    data['backend_route'] = "correct_token";
    console.log(data)
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
profileProject: function(data){
    data['backend_route'] = "order_profile";
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

searchToken: function(data){
    data['backend_route'] = "search_token";
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

getCorrectionSuggestions: function(data){
    data['backend_route'] = "get_correction_suggestions";
    console.log(data);
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

getAllCorrectionSuggestions: function(data){
    data['backend_route'] = "get_all_correction_suggestions";
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
getSplitImages: function(data){
    data['backend_route'] = "get_split_images";
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
getLexiconExtension: function(data){
  data['backend_route'] = "inspect_extended_lexicon";
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

startLexiconExtension: function(data){
  data['backend_route'] = "start_lexicon_extension";
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
getProtocol: function(data){
  data['backend_route'] = "inspect_postcorrection";
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
startPostcorrection: function(data){
  data['backend_route'] = "start_postcorrection";
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

profileWithExtensions: function(data){
  data['backend_route'] = "profile_le";
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

getJobs: function(data){
  data['backend_route'] = "get_jobs";
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
