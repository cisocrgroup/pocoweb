
define(["app","common/util","apps/users/home/home_view"], function(ResearchTool,Util){

ResearchTool.module("UsersApp.Home", function(Home, ResearchTool, Backbone, Marionette, $, _){

 Home.Controller = {

 	showHome: function(){

   		require(["entities/users"], function(){

   		var backdropView = new ResearchTool.Common.Views.LoadingBackdropOpc();
   		ResearchTool.backdropRegion.show(backdropView);
   		
		var currentUser = ResearchTool.request('app:currentUser');
    	var fetchingAuthCheck = ResearchTool.request("auth:authcheck");

		$.when(currentUser,fetchingAuthCheck).done(function(currentUser){
        backdropView.destroy();

		var usersHomeLayout = new Home.Layout({model:currentUser});
	 	var	usersHomeHeader = new Home.Header();
		var	usersHomeHub = new Home.Hub({model:currentUser});


			usersHomeLayout.on("show",function(){
			
				usersHomeLayout.headerRegion.show(usersHomeHeader);
				usersHomeLayout.contentRegion.show(usersHomeHub);

    		});


			
		usersHomeLayout.on("currentuser:loggedOut",function(){
			ResearchTool.UsersApp.Home.Controller.showHome();
 		}); // on:loggedOut

	
		
		ResearchTool.mainRegion.show(usersHomeLayout);
		}).fail(function(response){ 

 			      backdropView.destroy();
				  var errortext = Util.getErrorText(response);    
                  var errorView = new Home.Error({model: currentUser,errortext:errortext})

                  errorView.on("currentuser:loggedIn",function(){
					    ResearchTool.UsersApp.Home.Controller.showHome();
                  });

                  ResearchTool.mainRegion.show(errorView);    // $when

          }); //  $.when(fetchingAuth).done // $when fetchingUsers

		}); // require

	}
 }
});

return ResearchTool.UsersApp.Home.Controller;

});