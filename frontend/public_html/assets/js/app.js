// ======
// app.js
// ======

define(["bootstrap","datepicker","marionette","datatables.net-responsive","datatables.net-bs4","datatables_scroller","moment","common/util","common/views","tpl!common/templates/maintemplate.tpl",
], function(Bootstrap,Datepicker,Marionette,db_resp,bs_resp,data_scroller,moment,Util,Views,mainTpl){

var App = Marionette.Application.extend({
    region: "#app-region",
});

var App = new App();

 const MainView = Marionette.View.extend({
    regions:{
     headerRegion:"#header-region",
     mainRegion: '#main-region',
       footerRegion:"#footer-region",
       dialogRegion:"#dialog-region",
     backdropRegion:"#backdrop-region",
     addProjectRegion:"#add-project-region"

    },
    template: mainTpl,
    className:"sub-app-region"
  })



require(["backbone"], function (Backbone) {

// console.log(Backbone.VERSION)

});

App.navigate = function(route, options){
 options || (options = {});
Backbone.history.navigate(route, options);
};

App.getCurrentRoute = function(){
 return Backbone.history.fragment
};

App.getCurrentUser = function() {
  let pcw = localStorage.getItem('pcw');
  if (pcw != null) {
    return JSON.parse(pcw).user;
  }
  return null;
};

App.updateCurrentUser = function(user) {
  var data = JSON.parse(localStorage.getItem('pcw'));
  data.user = user;
  localStorage.setItem('pcw', JSON.stringify(data));
};

App.getAuthToken = function() {
  if (localStorage.getItem('pcw') === null) {
      return - 1;
  }
  return JSON.parse(localStorage.getItem('pcw')).auth;
};

App.clearCurrentUser = function() {
  localStorage.removeItem('pcw');
};

App.setLineHeight = function(id,val){
  var data = JSON.parse(localStorage.getItem('pcw_options'));
    if(data.lineHeights==null){
      data.lineHeights = {};
    }

  data.lineHeights[id] = val;
  localStorage.setItem('pcw_options', JSON.stringify(data));
};

App.getLineHeight = function(id){
  var data = JSON.parse(localStorage.getItem('pcw_options'));
    if(data.lineHeights!=null&&data.lineHeights[id]!=undefined){
      return data.lineHeights[id];
    }
    return 30;
};

App.getPageHits = function(id) {
  var data = JSON.parse(localStorage.getItem('pcw_options'));
    if(data.pageHits != null && data.pageHits[id]!=undefined){
      return data.pageHits[id];
    }
    return 8;
};

App.setPageHits = function(id, val){
  var data = JSON.parse(localStorage.getItem('pcw_options'));
  if(data.pageHits == null){
    data.pageHits = {};
  }
  data.pageHits[id] = val;
  localStorage.setItem('pcw_options', JSON.stringify(data));
};

App.setLineNumbers = function(id,val){
  var data = JSON.parse(localStorage.getItem('pcw_options'));
    if(data.lineNumbers==null){
      data.lineNumbers = {};
    }

  data.lineNumbers[id] = val;
  localStorage.setItem('pcw_options', JSON.stringify(data));
};

App.getLineNumbers = function(id){
  var data = JSON.parse(localStorage.getItem('pcw_options'));
    if(data.lineNumbers!=null&&data.lineNumbers[id]!=undefined){
      return data.lineNumbers[id];
    }
    return true;
};

App.getCharmapFilter = function() {
   return"abcdefghijklmnopqrstuvwxyz" +
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ" +
	"0123456789" +
	"\\'\":,-+/@#$.;<>(){}[]\\&?!=*^~_";
};

App.on("start", function(){

    // init options
    if (localStorage.getItem('pcw_options') === null) {
      var options = {lineHeights:{},lineNumbers:{}};
      console.log(options);
        localStorage.setItem('pcw_options',JSON.stringify(options));
    }

    if(Backbone.history){
       require([
               "apps/header/header_app"
               ,"apps/home/home_app"
               ,"apps/footer/footer_app"
               ,"apps/projects/projects_app"
               ,"apps/docs/docs_app"
               ,"apps/users/users_app"

        ], function (HeaderApp,HomeApp,FooterApp,ProjectsApp,DocsApp,UsersApp) {

          console.log(App)

    var app_region = App.getRegion();

      App.on("nav:login",function(asModal){

      require(["apps/users/login/login_view","entities/users","common/util"], function(Login,UserEntities,Util){


     var loginView = new Login.Form({asModal:asModal});

     if(asModal){

      App.mainLayout.showChildView('dialogRegion',loginView);
      $('#loginModal').modal();

     }
     else {

      App.mainLayout.showChildView('mainRegion',loginView);
     }


     loginView.on("login:submit",function(data){
      if(asModal) $('#loginModal').modal('hide');

    var loggingInUser = UserEntities.API.login(data);
                 $.when(loggingInUser).done(function(result){
                   localStorage.setItem('pcw', JSON.stringify(result));
                        App.mainmsg.updateContent("Login successful!",'success');

                         App.Navbar.options.user = App.getCurrentUser();
                         App.Navbar.render();

                          var currentRoute =  App.getCurrentRoute();
                          var page_re = /projects\/\d+.*/;
                          var page_route_found = App.getCurrentRoute().match(page_re);

                          if(page_route_found!=null){
                             var split = currentRoute.split("/")
                             App.trigger("projects:show",split[1],split[3])
                          }

                          switch(currentRoute) {
                            case "projects":
                              App.trigger("projects:list")
                              break;
                            case "users/list":
                              App.trigger("users:list")
                              break;
                            case "users/account":
                              App.trigger("users:show","account")
                              break;
                              case "users/new":
                              App.trigger("users:new")
                              break;
                            default:
                              App.trigger("home:portal")
                          }


                }).fail(function(response){
                   Util.defaultErrorHandling(response,'danger');
          }); //  $.when(loggingInUser).done

       });

     loginView.on("go:back",function(){
        App.trigger("home:portal");
     });

      });
    }); // login


    App.mainLayout = new MainView();
    App.mainmsg  = new Views.Message({id:"mainmsg",message:'Welcome to PoCoWeb. Please <a href="#" class="js-login">login</a>.',type:'info'});

     App.mainmsg.on("msg:login",function(data){
     App.trigger("nav:login",true);
     });

     App.mainmsg.on("msg:logout",function(data){
     App.trigger("nav:logout");
     });

     App.showView(App.mainLayout);

     HeaderApp.API.showHeader(function(){
       FooterApp.API.showFooter();
       Backbone.history.start();

       if(App.getCurrentRoute() === ""){

          App.trigger("home:portal");

       }

     });


     // App.on('page_changed',function(){
     //  console.log(App.getCurrentRoute())
     //  console.log("Page page_changed")
     //  $('#mainmsg').empty();
     // });


       });
    }
});



return App;

}); // define

var openFile = function(event) {
      var input = event.target;

      var reader = new FileReader();
      reader.onload = function(){
        var text = reader.result;
        var node = document.getElementById('output');
        node.innerText = text;
        console.log(reader.result.substring(0, 200));
      };
      reader.readAsText(input.files[0]);
    };