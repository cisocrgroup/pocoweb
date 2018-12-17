requirejs.config({
  baseUrl: "assets/js",
  paths: {

    backbone: "vendor/backbone",
    "backbone.radio": "vendor/backbone.radio",

    "backbone-validation": "vendor/backbone-validation",
    "backbone.syphon": "vendor/backbone.syphon",

    jquery: "vendor/jquery",
    "jquery-ui": "vendor/jquery-ui",

    "select2": "vendor/select2.min",
    "moment":"vendor/moment",
    marionette: "vendor/backbone.marionette",
    text: "vendor/text",

    spin: "vendor/spin",
    "spin.jquery": "vendor/spin.jquery",

    json2: "vendor/json2",


    tpl: "vendor/underscore-tpl",
    underscore: "vendor/underscore",


    bootstrap:"vendor/bootstrap.min",
    "datepicker":"vendor/datepicker",

    "datatables.net": "vendor/jquery.dataTables.min",
     datatables: "vendor/dataTables.bootstrap4.min",
     "datatables.net-responsive": "vendor/dataTables.responsive.min",
     "datatables.net-bs4": "vendor/responsive.bootstrap4.min",
     "datatables_scroller" : "vendor/dataTables.scroller.min"



    },

  shim: {
    underscore: {
      exports: "_"
    },

      "bootstrap":{
      deps: ["jquery"],
    },

    backbone: {
      deps: ["jquery", "underscore"],
      exports: "Backbone"
    },
    "backbone.syphon":{
     deps: ["backbone"],
     exports: "Backbone"
    },
    "backbone-validation":{
     deps: ["backbone"],
     exports: "Backbone"
    },
    "backbone.radio":{
     deps: ["backbone"],
     exports: "Backbone"
    },    

    marionette: {
      deps: ["backbone"],
      exports: "Marionette"
    },
   "jquery-ui": ["jquery"],
   bootstrap:{
    deps:["jquery"]
   },
    tpl: ["text"],
  
    // "datatables-foundation":{
    //     deps: ["jquery","datatables"],
    // },
    
      "datatables.net":{
      deps: ["jquery","bootstrap"],
    },

     "datatables":{
      deps: ["jquery","datatables.net"],
    },
      "datatables.net-responsive":{
      deps: ["jquery","datatables.net"],
    },
      "datatables.net-bs4":{
      deps: ["jquery","datatables.net"],
    },

   "datatables_scroller":{
      deps: ["jquery","datatables.net"],
    },

    "select2" :["jquery"],
    "moment" :["jquery"], 
    "datepicker":["jquery"],
 
    "spin.jquery": ["spin", "jquery"]   

   }
});


require(["app"], function(IPS_App){
   // IPS_App.start();
});
