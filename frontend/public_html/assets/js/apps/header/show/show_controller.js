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


  		headerShowLayout.on("show",function(){
  		headerShowLayout.navbarRegion.show(headerShowTopbar);
 		}); // on:show

  headerShowTopbar.on("nav:logout",function(data){

    var loggingOutUser = UserEntities.API.logout();


                 $.when(loggingOutUser).done(function(result){

                  if(App.getCurrentRoute()=="home"){


                        App.homeMsg.updateContent(result.message,'success');
                         headerShowTopbar.setLoggedOut();
                        }
              
                  
                }).fail(function(response){ 
                  App.homeMsg.updateContent(response.responseText,'danger');                       
                                      
          }); //  $.when(loggingOutUser).done

        
    });

  headerShowTopbar.on("nav:login",function(data){
     headerLogin = new Login.Form();
     App.mainLayout.showChildView('dialogRegion',headerLogin);
     $('#loginModal').modal();


     headerLogin.on("login:submit",function(data){
     $('#loginModal').modal('hide');

    var loggingInUser = UserEntities.API.login(data);


                 $.when(loggingInUser).done(function(result){

                  if(App.getCurrentRoute()=="home"){


                        App.homeMsg.updateContent(result.message,'success');
                         headerShowTopbar.setLoggedIn(result.user.name);
                        }
              
                  
                }).fail(function(response){ 
                  App.homeMsg.updateContent(response.responseText,'danger');                       
                                      
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
      App.homeMsg.updateContent("Welcome back to PoCoWeb: "+login_check.name+"!",'success');
      headerShowTopbar.setLoggedIn(login_check.name);
      }
    });
      headerShowTopbar.on("nav:exit",function(){
     App.trigger("home:portal");
    });


      App.mainLayout.showChildView('headerRegion',headerShowTopbar);

    }); //fetching user,util

    });
  
    } // showHeader

  }




return Controller;

});
