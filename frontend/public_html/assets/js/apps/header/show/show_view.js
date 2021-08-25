// =============================
// apps/header/show/show_view.js
// =============================

define(["marionette","app","common/views","common/util",
  "tpl!apps/header/show/templates/layout.tpl",
  "tpl!apps/header/show/templates/navbar.tpl",
  "tpl!common/templates/emptytemplate.tpl"

  ], function(Marionette,App,Views,Util,layoutTpl,navbarTpl,emptyTpl){

  var Show={};

 Show.Layout = Marionette.View.extend({
    template: layoutTpl,
    regions:{
      navbarRegion: "#navbar-region",
      msgRegion: "#msg-region"

    }


  });


   Show.Topbar = Marionette.View.extend({
   template: navbarTpl,
   triggers:{
      "click .js-logout":"nav:logout",
      "click .js-exit":"nav:exit",
      "click #help_button":"nav:help",
      "click .js-logs":"nav:logs"
     },

    events:{
      "click .js-login":"login_clicked",
      'click .nav_item.active' : 'nav_item_clicked',
      'click .logo_area_left' : 'nav_item_clicked'
    },
   

    login_clicked:function(){
      App.trigger("nav:login",true);
    },



      serializeData: function(){
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        data.navbarItems = Marionette.getOption(this,"navbarItems");
        data.version = Marionette.getOption(this,"version");
        data.user = Marionette.getOption(this,"user");
        // data.currentRoute = Marionette.getOption(this,"currentRoute");
        return data;
    }

  });




return Show;

});
