// ===========================
// apps/users/home/home_view.js
// ===========================

define(["marionette","app","common/views","common/util",

], function(Marionette,App,Views,Util){


  var Home = {};

  Home.Layout = Views.Layout.extend({
  });


 Home.Header = Views.Header.extend({
    initialize: function(){
        this.title = "User Management"
        this.icon ="fas fa-users-cog"
        this.color ="red"
      }
  });

 Home.Hub = Views.CardHub.extend({
  
 })

  Home.FooterPanel = Views.FooterPanel.extend({
    });

return Home;


});
