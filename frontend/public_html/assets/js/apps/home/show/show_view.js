// ===========================
// apps/home/home/home_view.js
// ===========================

define(["marionette","app","common/views","common/util",
 "tpl!apps/home/show/templates/layout.tpl",
 "tpl!apps/home/show/templates/header.tpl",

], function(Marionette,App,Views,Util,layoutTpl,headerTpl){


  var Home = {};

  Home.Layout =Marionette.View.extend({
  template: layoutTpl,
  regions:{
    headerRegion: "#hl-region",
    hubRegion: "#hub-region",
    msgRegion: "#msg-region"

  },
  className:"home_container"
 
  });


 Home.Header = Marionette.View.extend({
     template:headerTpl,
      triggers:{
      "click .js-login":"home:login"
     }
  });

 Home.Message = Views.Message.extend({
    triggers:{
      "click .js-login":"msg:login",
     },
  });

 Home.Hub = Views.CardHub.extend({
  
 })


return Home;


});
