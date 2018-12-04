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
    
}


};



return Entities;

});
