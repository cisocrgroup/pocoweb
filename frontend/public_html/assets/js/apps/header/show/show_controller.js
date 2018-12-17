// ===================================
// apps/header/show/show_controller.js
// ===================================

define(["app","common/util","apps/header/show/show_view"], function(IPS_App,Util,Show){


  var Controller = {

   showHeader: function(){


  

        var headerShowLayout = new Show.Layout();
 
 

      var headerShowTopbar = new Show.Topbar({});
  		headerShowLayout.on("show",function(){
  		headerShowLayout.navbarRegion.show(headerShowTopbar);
 		}); // on:show

  headerShowTopbar.on("nav:item_clicked",function(data){


        
    });



       headerShowTopbar.on("nav:help",function(){

            require(["entities/util"], function(UtilEntities){

                  var fetchingHelpTexts= UtilEntities.API.getHelpText(IPS_App.getCurrentRoute(),currentUser.get('userRole'));


                  $.when(fetchingHelpTexts).done(function(helptexts){
       

                  var helpModal = new Show.Help({
                    helpitems:helptexts.helpItems,
                    asModal:true
                  }); 

               IPS_App.mainLayout.showChildView('dialogRegion',helpModal);

            }); // when fetchingHelp

        }); // require
           
       });



      


      headerShowTopbar.on("nav:exit",function(){
     IPS_App.trigger("home:portal");
    });


      IPS_App.mainLayout.showChildView('headerRegion',headerShowTopbar);


 
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
