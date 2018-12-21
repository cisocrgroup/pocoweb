// ===================================
// apps/header/show/show_controller.js
// ===================================

define(["app","common/util","apps/header/show/show_view","apps/users/login/login_view"], function(App,Util,Show,Login){


  var Controller = {

   showHeader: function(){


          require(["entities/users"], function(UserEntities){


        var headerShowLayout = new Show.Layout();
         var headerLogin ;
 

      var headerShowTopbar = new Show.Topbar({});
  		headerShowLayout.on("show",function(){
  		headerShowLayout.navbarRegion.show(headerShowTopbar);
 		}); // on:show

  headerShowTopbar.on("nav:item_clicked",function(data){


        
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
                 
                  var newMsg = new Show.Message({message:result,type:'success'});

                  App.mainLayout._regions.mainRegion.currentView.showChildView('msgRegion',newMsg);
                  }

                  console.log(result)

                    // TO DO
                })

     console.log(data)
    });

    });

  



       headerShowTopbar.on("nav:help",function(){

            require(["entities/util"], function(UtilEntities){

                  var fetchingHelpTexts= UtilEntities.API.getHelpText(App.getCurrentRoute(),currentUser.get('userRole'));


                  $.when(fetchingHelpTexts).done(function(helptexts){
       

                  var helpModal = new Show.Help({
                    helpitems:helptexts.helpItems,
                    asModal:true
                  }); 

               App.mainLayout.showChildView('dialogRegion',helpModal);

            }); // when fetchingHelp

        }); // require
           
       });



      


      headerShowTopbar.on("nav:exit",function(){
     App.trigger("home:portal");
    });


      App.mainLayout.showChildView('headerRegion',headerShowTopbar);


    });
  
    } // showHeader

  }




  function getItemByUrl(navItems,url){
  
  var result;

        for (key in navItems){

          if(navItems[key].url==url){
            result= navItems[key];
          }

          else{

              for (key2 in navItems[key].nav_children){
                      if(navItems[key].nav_children[key2].url==url){
                      result= navItems[key].nav_children[key2];
                      }           
              }

          }

        }

return result;

}



return Controller;

});
