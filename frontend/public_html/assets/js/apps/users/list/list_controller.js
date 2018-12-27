
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

			var usersFooterPanel = new List.FooterPanel();

			    usersListLayout.showChildView('headerRegion',usersListHeader);
                usersListLayout.showChildView('contentRegion',usersListView);	
                usersListLayout.showChildView('panelRegion',usersFooterPanel);	

 		}); // on:show



         App.mainLayout.showChildView('mainRegion',usersListLayout);

		}).fail(function(response){ 

		      var mainRegion = App.mainLayout.getRegion('mainRegion');
		       mainRegion.empty();

		         App.mainmsg.updateContent(response.responseText,'danger');              
		                          
                         
                                        
          }); //  $.when(fetchingAuth).done // $when fetchingUsers

		}); // require
	}
 }


return Controller;

});