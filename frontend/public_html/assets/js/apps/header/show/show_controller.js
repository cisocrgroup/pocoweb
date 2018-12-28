// ===================================
// apps/header/show/show_controller.js
// ===================================

define(["app","common/util","apps/header/show/show_view","apps/users/login/login_view"], function(App,Util,Show,Login){


  var Controller = {

   showHeader: function(){


  require(["entities/users","entities/util"], function(UserEntities,UtilEntities){


    var fetchingVersion = UtilEntities.API.getApiVersion();
    var fetchingLoginCheck = UserEntities.API.loginCheck();
      $.when(fetchingVersion,fetchingLoginCheck).done(function(api_version,login_check){

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


                          switch(currentRoute) {
                            case "projects":
                              App.trigger("projects:list")
                              break;
                            case "users/list":
                              App.trigger("users:list")
                            case "users/account":
                              App.trigger("users:show","account")
                              break;
                            default:
                              // code block
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
       if(login_check!=-1){
      App.mainmsg.updateContent("Welcome back to PoCoWeb: "+login_check.name+"!",'success');
      headerShowTopbar.setLoggedIn(login_check.name);
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
