
define(["app","common/util","apps/users/list/list_view"], function(App,Util,List){


 var Controller = {

 	listUsers: function(){

   		require(["entities/users"], function(UserEntities){


       	var fetchingUsers = UserEntities.API.getUsers();

		usersListLayout = new List.Layout();

    	 $.when(fetchingUsers).done(function(users){
		usersListLayout.on("attach",function(){

			  var breadcrumbs = [
             {title:"<i class='fas fa-home'></i>",url:"/"},
             {title:"User Management",url:"#users"},
              {title:"List",url:""},

       		  ];


 			var usersListHeader = new List.Header({breadcrumbs:breadcrumbs});
			var usersListView = new List.UsersList({collection: users.users});
			var usersListPanel = new List.Panel();

			usersListView.on('user:delete',function(id,delete_row){

				var confirmModal = new List.AreYouSure({title:"Are you sure...",text:"...you want to delete user account "+id+" ?",id:"deleteModal"})
 				App.mainLayout.showChildView('dialogRegion',confirmModal)

 				confirmModal.on('delete:confirm',function(){
 					   	var deletingUser = UserEntities.API.deleteUser({id:id});
    		    	 	$('#deleteModal').modal("hide");
			    	 $.when(deletingUser).done(function(result){
			    	 	 App.mainmsg.updateContent("User account "+id+" successfully deleted.",'success',true,result.request_url);
			    	 	delete_row.remove();
			    	 }).fail(function(response){
     			         App.mainmsg.updateContent(response.responseText,'danger',true,response.request_url)
				      });
 				})



			});


			usersListPanel.on('user:create',function(){
				var userForm = new List.Form({model:new UserEntities.User(),asModal:true,id:"userModal",modaltitle:"Create a new user account",admincheck:true})
 				App.mainLayout.showChildView('dialogRegion',userForm)

				 userForm.on('form:submit',function(data){

				 	 var creatingUser = UserEntities.API.createUser(data);
			   	 	$.when(creatingUser).done(function(result){
			   	 		$('#userModal').modal('hide');
				         App.mainmsg.updateContent("Successfully created new user.",'success',true,result.request_url);

				         	var fetchingUsers = UserEntities.API.getUsers();

					    	 $.when(fetchingUsers).done(function(users_new){
								   usersListView.collection=users_new.users;
								   usersListView.options.collection=users_new.users;

					    	 	   usersListView.trigger("onAttach");
  					    	 	   usersListView.render();


					    	 });

				       }).fail(function(response){
   			   	 		$('#userModal').modal('hide');
     			         App.mainmsg.updateContent(response.responseText,'danger',true,response.request_url)
				 		});
 				 });



			});

			 var usersFooterPanel = new List.FooterPanel({title: "Back to User Management <i class='fas fa-users'></i>",manual:true});

		        usersFooterPanel.on("go:back",function(){
		         App.trigger("users:home");
		        });

			    usersListLayout.showChildView('headerRegion',usersListHeader);
                usersListLayout.showChildView('panelRegion',usersListPanel);
                usersListLayout.showChildView('infoRegion',usersListView);
                usersListLayout.showChildView('footerRegion',usersFooterPanel);

 		}); // on:attach



         App.mainLayout.showChildView('mainRegion',usersListLayout);

		}).fail(function(response){

		      Util.defaultErrorHandling(response,'danger');


          }); //  $.when(fetchingAuth).done // $when fetchingUsers

		}); // require
	}
 }


return Controller;

});
