
define(["app","common/util","apps/users/list/list_view"], function(App,Util,List){


 var Controller = {

 	listUsers: function(){
		
   		require(["entities/users"], function(UserEntities){

 
       	var fetchingUsers = UserEntities.API.getUsers();

		usersListLayout = new List.Layout();

    	 $.when(fetchingUsers).done(function(users){
		console.log(users);
		usersListLayout.on("attach",function(){

 			var usersListHeader = new List.Header();
			var usersListView = new List.UsersList({collection: users.users});


			    usersListLayout.showChildView('headerRegion',usersListHeader);
                usersListLayout.showChildView('contentRegion',usersListView);	

 		}); // on:show



         App.mainLayout.showChildView('mainRegion',usersListLayout);

		}).fail(function(response){ 


				  var errortext = Util.getErrorText(response);    
                  var errorView = new List.Error({model: currentUser,errortext:errortext})


                  App.mainRegion.show(errorView);   
                          
                         
                                        
          }); //  $.when(fetchingAuth).done // $when fetchingUsers

		}); // require
	}
 }


return Controller;

});