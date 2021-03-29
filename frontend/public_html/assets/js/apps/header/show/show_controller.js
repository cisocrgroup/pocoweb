// ===================================
// apps/header/show/show_controller.js
// ===================================

define(["app","common/util","apps/header/show/show_view","apps/users/login/login_view","apps/users/logs/logs_view"], function(App,Util,Show,Login,Logs){


  var Controller = {

   showHeader: function(){


  require(["entities/users","entities/util"], function(UserEntities,UtilEntities){


    var fetchingVersion = UtilEntities.API.getApiVersion();
    var fetchingLoginCheck = UserEntities.API.loginCheck();

      $.when(fetchingVersion,fetchingLoginCheck).done(function(api_version,login_check){
        var headerShowLayout = new Show.Layout();
        var headerLogin ;


      var headerShowTopbar = new Show.Topbar({
        version:api_version.version,
        user: login_check
      });
      App.Navbar = headerShowTopbar;

      // var headerShowMsg = new Show.Message({id:"mainmsg",message:'Welcome to PoCoWeb. Please <a href="#" class="js-login">login</a>.',type:'info'});
      // App.mainmsg = headerShowMsg; // save view to be changed form other views..


  		headerShowLayout.on("attach",function(){
  		headerShowLayout.showChildView('navbarRegion',headerShowTopbar);


      
 		}); // on:show

  headerShowTopbar.on("nav:logout",function(data){

    var loggingOutUser = UserEntities.API.logout();


                 $.when(loggingOutUser).done(function(result){
                  console.log(result)
                   App.mainmsg.updateContent("Logout successful",'success',true,result.request_url);
                   App.logout();
                   headerShowTopbar.options.user = App.getCurrentUser();
                   headerShowTopbar.render();

                   App.trigger('home:portal');

                }).fail(function(response){
                   App.logout();
                   headerShowTopbar.options.user = App.getCurrentUser();
                   headerShowTopbar.render();
                   Util.defaultErrorHandling(response,'danger',true,response.request_url);
          }); //  $.when(loggingOutUser).done


    });



  headerShowTopbar.on("attach",function(){
       if(login_check.id!=-1){
             var user = login_check;

      App.mainmsg.updateContent("Welcome back to PoCoWeb: '"+user.name+"'",'success',false);
        headerShowLayout.showChildView('msgRegion',App.mainmsg)
      }
      else {
        headerShowLayout.showChildView('msgRegion',App.mainmsg)
      }

    

    });
    headerShowTopbar.on("nav:exit",function(){
     App.trigger("home:portal");

    });

    headerShowTopbar.on("nav:logs",function(){
          var logs = App.getLastMessages(App.getCurrentUser()['id']);
          var userLogs = new Logs.List({collection: logs,asModal:true,table_id:"log_list_header"});
          App.mainLayout.showChildView('dialogRegion',userLogs);



    });




      App.mainLayout.showChildView('headerRegion',headerShowLayout);

    })
    })

    } // showHeader

  }




return Controller;

});
