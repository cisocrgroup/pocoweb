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
               ,"apps/ocrd/ocrd_app"
               ,"apps/docs/docs_app"

        ], function (HeaderApp,HomeApp,FooterApp,ProjectsApp,OcrdApp,DocsApp) {


    var app_region = App.getRegion();

    App.mainLayout = new MainView();

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