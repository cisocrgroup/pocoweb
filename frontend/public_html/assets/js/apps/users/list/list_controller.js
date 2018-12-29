
define(["app","common/util","apps/users/list/list_view"], function(App,Util,List){


 var Controller = {

 	listUsers: function(){
		
   		require(["entities/users"], function(UserEntities){

 
       	var fetchingUsers = UserEntities.API.getUsers();

		usersListLayout = new List.Layout();

    	 $.when(fetchingUsers).done(function(users){
		usersListLayout.on("attach",function(){

 			var usersListHeader = new List.Header();
			var usersListView = new List.UsersList({collection: users.users});

			usersListView.on('user:delete',function(id,delete_row){

				var confirmModal = new List.AreYouSure({title:"Are you sure...",text:"...you want to delete user account "+id+" ?",id:"deleteModal"})
 				App.mainLayout.showChildView('dialogRegion',confirmModal)

 				confirmModal.on('delete:confirm',function(){
 					   	var deletingUser = UserEntities.API.deleteUser({id:id});
			    	 $.when(deletingUser).done(function(result){
			    	 	$('#deleteModal').modal("hide");
			    	 	 App.mainmsg.updateContent("User account "+id+" successfully deleted.",'success');              
			    	 	delete_row.remove();
			    	 }).fail(function(response){ 
				        App.mainmsg.updateContent(response.responseText,'danger');    
 				})

			    

			});

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