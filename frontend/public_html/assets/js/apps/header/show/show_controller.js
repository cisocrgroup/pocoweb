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

      
      var headerShowMsg = new Show.Message({message:'Welcome to PoCoWeb. Please <a href="#" class="js-login">login</a>.',type:'info'});
  		headerShowLayout.on("attach",function(){
  		headerShowLayout.showChildView('navbarRegion',headerShowTopbar);
 		}); // on:show

  headerShowTopbar.on("nav:logout",function(data){

    var loggingOutUser = UserEntities.API.logout();


                 $.when(loggingOutUser).done(function(result){

                  if(App.getCurrentRoute()=="home"){


                       headerShowMsg.updateContent(result.message,'success');
                         headerShowTopbar.setLoggedOut();
                        }
              
                  
                }).fail(function(response){ 
                  headerShowMsg.updateContent(response.responseText,'danger');                       
                                      
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

                  if(App.getCurrentRoute()=="home"){


                        headerShowMsg.updateContent(result.message,'success');
                         headerShowTopbar.setLoggedIn(result.user.name);
                        }
              
                  
                }).fail(function(response){ 
                  headerShowMsg.updateContent(response.responseText,'danger');                       
                                      
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
      headerShowMsg.updateContent("Welcome back to PoCoWeb: "+login_check.name+"!",'success');
      headerShowTopbar.setLoggedIn(login_check.name);
        headerShowLayout.showChildView('msgRegion',headerShowMsg)

      }
      else {
        headerShowLayout.showChildView('msgRegion',headerShowMsg)

  

      }
    });
      headerShowTopbar.on("nav:exit",function(){
     App.trigger("home:portal");
    });


          headerShowMsg.on("msg:login",function(data){
        headerShowTopbar.trigger("nav:login");
        });

      headerShowMsg.on("msg:logout",function(data){
       headerShowTopbar.trigger("nav:logout");
        });



      App.mainLayout.showChildView('headerRegion',headerShowLayout);

    }); //fetching user,util

    });
  
    } // showHeader

  }




return Controller;

});
