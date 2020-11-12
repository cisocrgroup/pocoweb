
define(["app","common/util","apps/users/new/new_view"], function(App,Util,New){


 var Controller = {

 	newUser: function(){
		
   		require(["entities/users"], function(UserEntities){

 
       	var fetchingUsers = UserEntities.API.getUsers();

		usersNewLayout = new New.Layout();

    	 $.when(fetchingUsers).done(function(users){
		usersNewLayout.on("attach",function(){

 			var usersNewHeader = new New.Header();
			var userForm = new New.Form({model:new UserEntities.User(),asModal:false,admincheck:true,create:true})

				 userForm.on('form:submit',function(data){

				 	 var creatingUser = UserEntities.API.createUser(data);
			   	 	$.when(creatingUser).done(function(result){
			   	 		$('#userModal').modal('hide');
			   	 		console.log(result)
				         App.mainmsg.updateContent("Successfully created new user.",'success');

				         App.trigger('users:list');

				       }).fail(function(response){
   			   	 		$('#userModal').modal('hide');
     			         App.mainmsg.updateContent(response.responseText,'danger')
				 		});    
 				 });

			    

		

			 var usersFooterPanel = new New.FooterPanel({title: "Back to User Management <i class='fas fa-users'></i>",manual:true});

		        usersFooterPanel.on("go:back",function(){
		         App.trigger("users:home");
		        });


			    usersNewLayout.showChildView('headerRegion',usersNewHeader);
                usersNewLayout.showChildView('contentRegion',userForm);	
                usersNewLayout.showChildView('panelRegion',usersFooterPanel);	

 		}); // on:attach



         App.mainLayout.showChildView('mainRegion',usersNewLayout);

		}).fail(function(response){ 

      			 Util.defaultErrorHandling(response,'danger');                        

		                                                  
                                        
          }); //  $.when(fetchingAuth).done // $when fetchingUsers

		}); // require
	}
 }


return Controller;

});