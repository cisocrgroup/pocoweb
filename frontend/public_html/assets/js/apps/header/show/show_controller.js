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
        console.log(login_check)
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
                   App.clearCurrentUser();
                   App.mainmsg.updateContent(result.message,'success');
                         headerShowTopbar.setLoggedOut();
                         App.trigger('home:portal');

                }).fail(function(response){
                   App.clearCurrentUser();
                  App.mainmsg.updateContent(response.responseText,'danger');
          }); //  $.when(loggingOutUser).done


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
             var user = login_check;

      App.mainmsg.updateContent("Welcome back to PoCoWeb: "+user.name+"!",'success');
      headerShowTopbar.setLoggedIn(user.name);
        headerShowLayout.showChildView('msgRegion',App.mainmsg)

      }
      else {
        headerShowLayout.showChildView('msgRegion',App.mainmsg)
      }
    });
      headerShowTopbar.on("nav:exit",function(){
     App.trigger("home:portal");
    });





      App.mainLayout.showChildView('headerRegion',headerShowLayout);

    }); //fetching user,util

    });

    } // showHeader

  }




return Controller;

});
