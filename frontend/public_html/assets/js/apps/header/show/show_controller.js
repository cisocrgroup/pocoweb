// ===================================
// apps/header/show/show_controller.js
// ===================================

define(["app","common/util","apps/header/show/show_view","apps/users/login/login_view"], function(App,Util,Show,Login){


  var Controller = {

   showHeader: function(){


  require(["entities/users","entities/util"], function(UserEntities,UtilEntities){


    var fetchingVersion = UtilEntities.API.getApiVersion();
    var fetchingLoginCheck = UserEntities.API.loginCheck();
      $.when(fetchingVersion,fetchingLoginCheck).done(function(api_version,logged_in_user){

        var headerShowLayout = new Show.Layout();
         var headerLogin ;
 

      var headerShowTopbar = new Show.Topbar({version:api_version.version});
      App.Navbar = headerShowTopbar;

      
      // var headerShowMsg = new Show.Message({id:"mainmsg",message:'Welcome to PoCoWeb. Please <a href="#" class="js-login">login</a>.',type:'info'});
      // App.mainmsg = headerShowMsg; // save view to be changed form other views..


  		headerShowLayout.on("attach",function(){
  		headerShowLayout.showChildView('navbarRegion',headerShowTopbar);
 		}); // on:show

  headerShowTopbar.on("nav:logout",function(data){

    var loggingOutUser = UserEntities.API.logout();


                 $.when(loggingOutUser).done(function(result){

                       App.mainmsg.updateContent(result.message,'success');
                         headerShowTopbar.setLoggedOut();
                        
                  
                }).fail(function(response){ 
                  App.mainmsg.updateContent(response.responseText,'danger');                       
                                      
          }); //  $.when(loggingOutUser).done

        
    });

  headerShowTopbar.on("nav:login",function(){
     headerLogin = new Login.Form();
     App.mainLayout.showChildView('dialogRegion',headerLogin);
     $('#loginModal').modal();


     headerLogin.on("login:submit",function(data){
     $('#loginModal').modal('hide');

    var loggingInUser = UserEntities.API.login(data);


                 $.when(loggingInUser).done(function(result){
                                            
                        App.mainmsg.updateContent(result.message,'success');
                         headerShowTopbar.setLoggedIn(result.user.name);
                          
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

  



       headerShowTopbar.on("nav:help",function(){

        //     require(["entities/util"], function(UtilEntities){

        //           var fetchingHelpTexts= UtilEntities.API.getHelpText(App.getCurrentRoute(),currentUser.get('userRole'));


        //           $.when(fetchingHelpTexts).done(function(helptexts){
       

        //           var helpModal = new Show.Help({
        //             helpitems:helptexts.helpItems,
        //             asModal:true
        //           }); 

        //        App.mainLayout.showChildView('dialogRegion',helpModal);

        //     }); // when fetchingHelp

        // }); // require
           
       });



     

  headerShowTopbar.on("attach",function(){
       if(logged_in_user!=-1){
      App.mainmsg.updateContent("Welcome back to PoCoWeb: "+logged_in_user.name+"!",'success');
      headerShowTopbar.setLoggedIn(logged_in_user.name);
        headerShowLayout.showChildView('msgRegion',App.mainmsg)

      }
      else {
        headerShowLayout.showChildView('msgRegion',App.mainmsg)
      }
    });
      headerShowTopbar.on("nav:exit",function(){
     App.trigger("home:portal");
    });


          App.mainmsg.on("msg:login",function(data){
        headerShowTopbar.trigger("nav:login");
        });

      App.mainmsg.on("msg:logout",function(data){
       headerShowTopbar.trigger("nav:logout");
        });



      App.mainLayout.showChildView('headerRegion',headerShowLayout);

    }); //fetching user,util

    });
  
    } // showHeader

  }




return Controller;

});
