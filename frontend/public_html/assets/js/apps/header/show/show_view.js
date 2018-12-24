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


 Show.Message = Views.Message.extend({
    triggers:{
      "click .js-login":"msg:login",
      "click .js-logout":"msg:logout"

     },
  });


   Show.Topbar = Marionette.View.extend({
   template: navbarTpl,
   triggers:{
      "click .js-logout":"nav:logout",
      "click .js-login":"nav:login",
      "click .js-exit":"nav:exit",
      "click #help_button":"nav:help"
     },
   
    events:{
      'click .nav_item.active' : 'nav_item_clicked',
      'click .logo_area_left' : 'nav_item_clicked'

    },


    
      serializeData: function(){
          var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
          data.navbarItems = Marionette.getOption(this,"navbarItems");
          data.version = Marionette.getOption(this,"version");

          // data.currentRoute = Marionette.getOption(this,"currentRoute");
        return data;
    
    },
    setLoggedIn: function(name){
    
          $('.right-nav').empty();
          $('.right-nav').prepend('<li class="nav-item js-logout"><a href="#" class="nav-link">Logout</a></li>');
          $('.right-nav').prepend('<li><p class="navbar-text" style="margin:0;">Logged in as user: '+name+" </p></li>");
        },
     setLoggedOut: function(name){
      $('.right-nav').empty();
      $('.right-nav').append('<li class="nav-item js-login"><a class="nav-link" href="#"><i class="fas fa-sign-in-alt fa-sm"></i> Login</a></li>');
    }
  


  });




return Show;

});

