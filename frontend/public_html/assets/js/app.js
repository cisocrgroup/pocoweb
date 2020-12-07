// ======
// app.js
// ======

define(["bootstrap","datepicker","jquery-ui","marionette","datatables.net-responsive","datatables.net-bs4","datatables_scroller","moment","common/util","common/views","tpl!common/templates/maintemplate.tpl",
], function(Bootstrap,Datepicker,jqueryui,Marionette,db_resp,bs_resp,data_scroller,moment,Util,Views,mainTpl){

var App = Marionette.Application.extend({
    region: "#app-region",
});

App = new App();

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

App.Navbar = "";

App.navigate = function(route, options){
 options || (options = {});

 if (options['back']){
 }
 Backbone.history.navigate(route);
};

App.getCurrentRoute = function(){
 return Backbone.history.fragment
};

App.newPcw = function() {
  return {
    user: {id: -1},
    auth: -1,
    options: {
      lineHeights: {}, // id: lineHeight
      pageHits: {},    // id: pageHits
      lineNumbers: {}, // id: lineNumber
      ignoreCase: {},  // id: ignore case
      hideCorrections: {},  // id: hide corrections
      confidenceThreshold : {}, //id: confidence threshold
      charMapFilter: "abcdefghijklmnopqrstuvwxyz" +
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZ" +
	    "0123456789" +
	    "\\'\":,-+/@#$.;<>(){}[]\\&?!=*^~_"
    }
  };
};

App.getPcw = function() {
  let pcw = JSON.parse(localStorage.getItem('pcw')) || App.newPcw();
  return pcw;
};

App.setPcw = function(pcw) {
  localStorage.setItem('pcw', JSON.stringify(pcw));
};

App.getCurrentUser = function() {
  return App.getPcw().user;
};

App.setCurrentUser = function(user) {
  let pcw = App.getPcw();
  pcw.user = user;
  App.setPcw(pcw);
};

App.setAuthToken = function(auth) {
  let pcw = App.getPcw();
  pcw.auth = auth;
  App.setPcw(pcw);
};

App.getAuthToken = function() {
  return App.getPcw().auth;
};

App.clearAuthToken = function() {
  App.setAuthToken("");
};

App.logout = function() {
  App.clearCurrentUser();
  App.clearAuthToken();
};

App.clearCurrentUser = function() {
  let pcw = App.getPcw();
  pcw.user = {id:-1};
  App.setPcw(pcw);
};

App.setLineHeight = function(id,val){
  let pcw = App.getPcw();
  pcw.options.lineHeights[id] = val;
  App.setPcw(pcw);
}

App.getLineHeight = function(id){
  let height = App.getPcw().options.lineHeights[id] || 30;
  return height;
};

App.getPageHits = function(id) {
  let pageHits = App.getPcw().options.pageHits[id] || 8;
  return pageHits;
};

App.setPageHits = function(id, val){
  let pcw = App.getPcw();
  pcw.options.pageHits[id] = val;
  App.setPcw(pcw);
};

App.setLineNumbers = function(id,val){
  let pcw = App.getPcw();
  pcw.options.lineNumbers[id] = val;
  App.setPcw(pcw);
};

App.getLineNumbers = function(id){
  let lineNumbers = App.getPcw().options.lineNumbers[id];
  if(lineNumbers!=undefined){
    return lineNumbers
  }
  return true;
};

App.getConfidenceThreshold = function(id) {
  let confidenceThreshold = App.getPcw().options.confidenceThreshold[id] || 0;
  return confidenceThreshold;
};

App.setConfidenceThreshold = function(id, val){
  let pcw = App.getPcw();
  pcw.options.confidenceThreshold[id] = val;
  App.setPcw(pcw);
};


App.getIgnoreCase = function(id) {
  let ignoreCase = App.getPcw().options.ignoreCase[id] || true;
  return ignoreCase;
};

App.setIgnoreCase = function(id, ic) {
  let pcw = App.getPcw();
  pcw.options.ignoreCase[id] = ic;
  App.setPcw(pcw);
};

App.getHideCorrections = function(id) {
  let hideCorrections = App.getPcw().options.hideCorrections[id] || false;
  return hideCorrections;
};

App.setHideCorrections = function(id, ic) {
  let pcw = App.getPcw();
  pcw.options.hideCorrections[id] = ic;
  App.setPcw(pcw);
};

App.getCharmapFilter = function() {
  return App.getPcw().options.charMapFilter;
};

App.on("start", function(){
  // init pcw local storage configuration
  let pcw = App.getPcw();
  App.setPcw(pcw);
  console.log(pcw);

    if(Backbone.history){
       require([
               "apps/header/header_app"
               ,"apps/home/home_app"
               ,"apps/footer/footer_app"
               ,"apps/projects/projects_app"
               ,"apps/docs/docs_app"
               ,"apps/users/users_app"

        ], function (HeaderApp,HomeApp,FooterApp,ProjectsApp,DocsApp,UsersApp) {
 Backbone.history.start();
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
                   App.setCurrentUser(result.user);
                   App.setAuthToken(result.auth);
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

                          else {

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
      

       if(App.getCurrentRoute().startsWith("user-content")){
          App.trigger("docs:show");
       }


       if(App.getCurrentRoute() === ""){
          App.trigger("home:portal");
       }

     });


     App.on('page_changed',function(){
      // $('#mainmsg').empty();
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
