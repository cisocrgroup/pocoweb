// ===========================
// apps/docs/home/home_view.js
// ===========================

define(["marionette","app","common/views","common/util"
], function(Marionette,App,Views,Util){


  var Home = {};

  Home.Layout = Views.Layout.extend({
  });


 Home.Header = Views.Header.extend({
    initialize: function(){
        this.title = "PocoWeb Documentation"
        this.icon ="fas fa-book"
        this.color ="purple"
      }
  });

 Home.Hub = Views.CardHub.extend({

 })

  Home.FooterPanel = Views.FooterPanel.extend({
    });

return Home;


});
