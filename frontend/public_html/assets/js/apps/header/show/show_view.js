// =============================
// apps/header/show/show_view.js
// =============================

define(["marionette","app","common/util",
  "tpl!apps/header/show/templates/layout.tpl",
  "tpl!apps/header/show/templates/navbar.tpl",
  "tpl!common/templates/emptytemplate.tpl"

  ], function(Marionette,IPS_App,Util,layoutTpl,navbarTpl,emptyTpl){

  var Show={};

 Show.Layout = Marionette.View.extend({
    template: layoutTpl,
    regions:{
      navbarRegion: "#navbar-region"
    }
  

  });

   Show.Topbar = Marionette.View.extend({
   template: navbarTpl,
   triggers:{
      "click .js-logout":"nav:logout",
      "click .js-exit":"nav:exit",
      "click #help_button":"nav:help"
     },
   
    events:{
      'click .nav_item.active' : 'nav_item_clicked',
      'click .logo_area_left' : 'nav_item_clicked',

    },
    
      serializeData: function(){
          var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
          data.navbarItems = Marionette.getOption(this,"navbarItems");
          // data.currentRoute = Marionette.getOption(this,"currentRoute");
        return data;
    
    },
  
   


  });




return Show;

});

