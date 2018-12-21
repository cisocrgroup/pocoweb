
define(["app","common/util","apps/users/list/list_view"], function(App,Util){


 var Controller = {

 	listUsers: function(){
		
   		require(["entities/users"], function(){

   		var backdropView = new App.Common.Views.LoadingBackdropOpc();
   		App.backdropRegion.show(backdropView);
   		
		var currentUser = App.request('app:currentUser');
    	var fetchingUsers = App.request("user:entities");
    	var fetchingAuthCheck = App.request("auth:authcheck");

		usersListLayout = new List.Layout({model:currentUser});

    	 $.when(fetchingUsers,fetchingAuthCheck).done(function(users){
     	 	backdropView.destroy();
	
		usersListLayout.on("show",function(){

 			var usersListHeader = new List.Header();
			var usersListView = new List.UsersList({collection: users});

			  usersListLayout.headerRegion.show(usersListHeader);
			  usersListLayout.contentRegion.show(usersListView);	

 		}); // on:show

		usersListLayout.on("currentuser:loggedOut",function(){
			App.UsersApp.List.Controller.listUsers();
 		}); // on:loggedOut


		App.mainRegion.show(usersListLayout);

		}).fail(function(response){ 


 			      backdropView.destroy();
				  var errortext = Util.getErrorText(response);    
                  var errorView = new List.Error({model: currentUser,errortext:errortext})

                  errorView.on("currentuser:loggedIn",function(){
					    App.UsersApp.List.Controller.listUsers();
                  });

                  App.mainRegion.show(errorView);   
                          
                         
                                        
          }); //  $.when(fetchingAuth).done // $when fetchingUsers

		}); // require
	}
 }


return Controller;

});