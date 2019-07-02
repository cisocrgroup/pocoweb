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
    className:"sub-app-region",

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



App.on("start", function(){


    if(Backbone.history){
       require([
               "apps/header/header_app"
               ,"apps/home/home_app"
               ,"apps/footer/footer_app"
               ,"apps/projects/projects_app"
               ,"apps/docs/docs_app"
               ,"apps/users/users_app"

        ], function (HeaderApp,HomeApp,FooterApp,ProjectsApp,DocsApp,UsersApp) {


    var app_region = App.getRegion();

      App.on("nav:login",function(asModal){

      require(["apps/users/login/login_view","entities/users","common/util"], function(Login,UserEntities,Util){


        console.log(asModal);
     headerLogin = new Login.Form();
     App.mainLayout.showChildView('dialogRegion',headerLogin);
     $('#loginModal').modal();


     headerLogin.on("login:submit",function(data){
     $('#loginModal').modal('hide');

    var loggingInUser = UserEntities.API.login(data);


                 $.when(loggingInUser).done(function(result){
                                            
                        App.mainmsg.updateContent(result.message,'success');
                         Util.setLoggedIn(result.user.name);
                          
                          var currentRoute =  App.getCurrentRoute();
                          var page_re = /projects\/\d+\/page\/.*/;
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
                            default:
                          } 

                                            
                }).fail(function(response){ 
                  App.mainmsg.updateContent(response.responseText,'danger');                       
                                      
          }); //  $.when(loggingInUser).done

       });
      });
    });


    App.mainLayout = new MainView();
    App.mainmsg  = new Views.Message({id:"mainmsg",message:'Welcome to PoCoWeb. Please <a href="#" class="js-login">login</a>.',type:'info'});

     App.showView(App.mainLayout);

     HeaderApp.API.showHeader(function(){
       FooterApp.API.showFooter();
        Backbone.history.start();

       if(App.getCurrentRoute() === ""){
          App.trigger("home:portal");

       }

     });




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