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
  projectId:"",
     }
  });

Entities.Project = Backbone.Model.extend({
     urlRoot: "projects",
     defaults:{
  author:null,
  books:null,
  language:null,
  p_model:null,
  profilerUrl:"default",
  projectId:null,
  title:"",
  user:"",
  histPatterns:"",
  year:"2018",
  pooled:true
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

  getPage: function(data){
    var defer = jQuery.Deferred();
    $.ajax({
      headers: {
        'Accept': 'application/json'
      },
      url: "rest/books/" + data.pid + "/pages/" + data.page + "?auth=" + App.getAuthToken(),
      type: "GET",
      success: function(data) {
        data['request_url'] = this.url;
        defer.resolve(new Entities.Page(data));
      },
      error: function(data){
        data['request_url'] = this.url;
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
          data['request_url'] = this.url;
          defer.resolve(new Entities.Project(data));
        },
        error: function(data){
          data['request_url'] = this.url;
          defer.reject(data);
        }
  });
  return defer.promise();
},

getProjectStatistics: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
        headers: {
          'Accept': 'application/json'
        },
        url: "rest/books/" + data.pid + "/statistics?auth=" + App.getAuthToken(),
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

	uploadProjectData: function(data){
		let defer = jQuery.Deferred();
        let formData = new FormData(data);
        let params = jQuery.param({
             "auth": App.getAuthToken(),
             "title": formData.get('title'),
             "author": formData.get('author'),
             "profilerUrl": formData.get('profilerUrl'),
             "language": formData.get('language'),
             "year": formData.get('year'),
             "histPatterns": formData.get('histPatterns')
        });
        let zip = formData.get('archive');
		$.ajax({
			headers: {
				'Content-Type': 'application/zip',
                'Accept': 'application/json'
			},
			url: "rest/books?" + params,
			type: "POST",
			data: zip,
			cache: false,
			processData: false,
			contentType: false,
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

	getAdaptiveTokens: function(data){
		var defer = jQuery.Deferred();
		$.ajax({
			headers: {
				'Accept': 'application/json'
			},
			url: "rest/profile/adaptive/books/" +
				data.pid + "?auth=" + App.getAuthToken(),
			type: "GET",
			cache:false,
			processData:false,
			contentType: false,
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

downloadProject: function(data){
    var defer = jQuery.Deferred();
     $.ajax({
       url: "rest/books/"+ data.pid + "/download" + "?auth=" + App.getAuthToken(),
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


  deleteProject: function(data){
    var defer = jQuery.Deferred();
    $.ajax({
      url: "rest/books/" + data.pid + "?auth=" + App.getAuthToken(),
      type: "DELETE",
      data: data,
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

  updateProject: function(data){
    data['projectdata']['year'] = parseInt(data['projectdata']['year']);
    console.log(data)
    var defer = jQuery.Deferred();
    $.ajax({
      headers: {
        'Accept': 'application/json',
        'Content-Type': 'application/json'
      },
      url: "rest/books/" + data.pid + "?auth=" + App.getAuthToken(),
      type: "PUT",
      data: JSON.stringify(data['projectdata']),
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
   splitProject: function(data){
    console.log(data)
    var defer = jQuery.Deferred();
       $.ajax({
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
        },
        url: "rest/pkg/split/books/" + data.pid + "?auth=" + App.getAuthToken(),
        type: "POST",
        data: JSON.stringify(data),
        success: function(data) {
              data['request_url'] = this.url;
              defer.resolve();
        },
         error: function(data){
           data['request_url'] = this.url;
           defer.reject(data);
         }
    });
    return defer.promise();
  },

assignPackageTo: function(data){
  console.log(data);
    var defer = jQuery.Deferred();
     $.ajax({
      url: "rest/pkg/assign/books/" + data.pid +
         "?auth=" + App.getAuthToken() + "&assignto="+data.uid,
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

assignPackageBack: function(data){
  console.log(data);
    var defer = jQuery.Deferred();
     $.ajax({
      url: "rest/pkg/assign/books/" + data.pid +
         "?auth=" + App.getAuthToken(),
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

takeBackPackages: function(data){
  console.log(data);
    var defer = jQuery.Deferred();
     $.ajax({
      url: "rest/pkg/takeback/books/" + data.pid +
         "?auth=" + App.getAuthToken(),
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
  correctLine: function(data){
    console.log(data)
    var defer = jQuery.Deferred();
       $.ajax({
       headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
        },
        url: "rest/books/"+data.pid+"/pages/"+data.page_id+"/lines/"+data.line_id+"?auth=" + App.getAuthToken()+"&t=manual",
        type: "PUT",
        data:JSON.stringify({correction:data.text}),
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
    correctToken: function(data){
    console.log(data)
    var defer = jQuery.Deferred();
       $.ajax({
          headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
        },
        url: "rest/books/"+data.pid+"/pages/"+data.page_id+"/lines/"+data.line_id+"/tokens/"+data.token_id+"?auth=" + App.getAuthToken()+"&t=manual",
        type: "PUT",
        data:JSON.stringify({correction:data.token}),
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

search: function(data){
    var defer = jQuery.Deferred();
      $.ajax({
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
        },
        url: "rest/books/" + data.pid + "/search?auth=" + App.getAuthToken()+"&q="+data.q+"&t="+data.searchType+"&skip="+data.skip+"&max="+data.max+"&i="+App.getIgnoreCase(data.pid),
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

getCorrectionSuggestions: function(data){
    var defer = jQuery.Deferred();
      $.ajax({
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
        },
        url: "rest/profile/books/" + data.pid + "?auth=" + App.getAuthToken()+"&q="+data.q,
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


getSuspiciousWords: function(data){

  var defer = jQuery.Deferred();
      $.ajax({
      headers: {
        'Accept': 'application/json'
      },
      url: "rest/profile/suspicious/books/" + data.pid + "?auth=" + App.getAuthToken(),
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
getErrorPatterns: function(data){

  var defer = jQuery.Deferred();
     $.ajax({
      headers: {
        'Accept': 'application/json'
      },
      url: "rest/profile/patterns/books/" + data.pid + "?auth=" + App.getAuthToken()+"&ocr=1",
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

getLexiconExtension: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
      headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/postcorrect/le/books/"+data.pid+"?auth=" + App.getAuthToken(),
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

putLexiconExtension: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
      headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/postcorrect/le/books/"+data.pid+"?auth=" + App.getAuthToken(),
      type: "PUT",
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

startLexiconExtension: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
         headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/postcorrect/le/books/"+data.pid+"?auth=" + App.getAuthToken(),
      type: "POST",
       data:data,
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

getPostcorrection: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
     headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/postcorrect/books/"+data.pid+"?auth=" + App.getAuthToken(),
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

startPostcorrection: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
     headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/postcorrect/books/"+data.pid+"?auth=" + App.getAuthToken(),
      type: "POST",
      data:data,
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


startTrainPostcorrection: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
     headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/postcorrect/train/books/"+data.pid+"?auth=" + App.getAuthToken(),
      type: "POST",
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


getPostcorrectionModels: function(data){
  var defer = jQuery.Deferred();
      $.ajax({
     headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/postcorrect/models/books/"+data.pid+"?auth=" + App.getAuthToken(),
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

setPostcorrectionModel: function(data,pid){
  var defer = jQuery.Deferred();
      $.ajax({
     headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json'
      },
      url: "rest/postcorrect/models/books/"+pid+"?auth=" + App.getAuthToken(),
      type: "POST",
      data:JSON.stringify(data),
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

  getCharmap: function(data){
    var defer = jQuery.Deferred();
    let url = "rest/books/" + data.pid + "/charmap?auth=" + App.getAuthToken()+
        "&filter="+encodeURIComponent(App.getCharmapFilter());
    $.ajax({
      headers: {
        'Accept': 'application/json'
      },
      url: url,
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

getGlobalPool: function(data){
    var defer = jQuery.Deferred();
    let url = "rest/pool/global?auth=" + App.getAuthToken();
    $.ajax({
      headers: {
        'Accept': 'application/json'
      },
      url: url,
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

  getOwnPool: function(data){
    var defer = jQuery.Deferred();
    let url = "rest/pool/user?auth=" + App.getAuthToken();
    $.ajax({
      headers: {
        'Accept': 'application/json'
      },
      url: url,
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
  }


};




return Entities;

});
