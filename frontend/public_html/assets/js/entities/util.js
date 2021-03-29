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
             data['request_url'] = this.url;
             defer.resolve(data);

            },
            error: function(data){
              data['request_url'] = this.url;
              defer.resolve(data);
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
    getLanguages: function(){
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'application/json'
         },
         url: "rest/profile/languages?auth="+App.getAuthToken(),
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
  getDocumentation: function(doc_type){
    var defer = jQuery.Deferred();
       $.ajax({
         headers: {
           'Accept': 'text/html'
         },
         url: doc_type+".html",
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

addPostCorrectionClassification: function(data) {
  // add never, always and sometimes
  let types = {}; // {'normalized': [vals...]}
  for (let key in data.corrections) {
    let val = data.corrections[key];
    let norm = val.normalized;
    if (!(norm in types)) {
      types[norm] = [];
    }
    types[norm].push(val);
  }

  // split types into sometimes, always and never
  let classify = function(vals) {
                    let all = true;
                    let none = true;
                    vals.forEach(function(val){
                      if (val.taken) {
                        none = false;
                      } else {
                        all = false;
                      }
                    });
                    if (all) {
                      return 'always';
                    }
                    if (none) {
                      return 'never';
                    }
                    return 'sometimes';
                  };
  data.never = {};
  data.always = {};
  data.sometimes = {};
  for (let key in types) {
    let klass = classify(types[key]);
    data[klass][key] = types[key];
  }
  return data;
}

};



return Entities;

});
