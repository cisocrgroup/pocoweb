
define(["app","common/util","apps/users/show/show_view"], function(App,Util,Show){


 var Controller = {

		showUser: function(id){

	   		require(["entities/users"], function(UserEntitites){

	   		// var backdropView = new App.Common.Views.LoadingBackdropOpc();
	   		// App.backdropRegion.show(backdropView);


			var fetchingUser = UserEntitites.API.getUser(id);

			$.when(fetchingUser).done(function(user){
			// backdropView.destroy();

			var userModel = new UserEntitites.User(user);
			console.log(userModel)

			var userShowLayout = new Show.Layout();

			var userShowHeader;
     		var userShowPanel;
			var userShowForm;
	
			userShowLayout.on("attach",function(){
			  

			  userShowHeader = new Show.Header({model:userModel});
			  userShowPanel = new Show.Panel({model:user});
			  userShowForm = new Show.Form({model:userModel});
			 
		       userShowLayout.showChildView('headerRegion',userShowHeader);
   		       userShowLayout.showChildView('panelRegion',userShowPanel);
			   userShowLayout.showChildView('infoRegion',userShowForm);


			   userShowPanel.on("show:back",function(){
			  	 App.trigger("users:list");
			  });

			  userShowPanel.on("show:update",function(data){
			  		data['id'] = user['id'];
			  		var updatingUser = UserEntitites.API.updateUser(data);
						$.when(updatingUser).done(function(user){
							   App.mainmsg.updateContent("Account updated successfully.",'success');      
							   $('.loginname').text(user.name);       

						});

			  });
 			userShowPanel.on("show:delete",function(){

 				var confirmModal = new Show.AreYouSure({title:"Are you sure...",text:"...you want to delete your account?",id:"deleteModal"})
 				App.mainLayout.showChildView('dialogRegion',confirmModal)
			  });


    		}); // show





         App.mainLayout.showChildView('mainRegion',userShowLayout);


    		}).fail(function(response){ 

		       var mainRegion = App.mainLayout.getRegion('mainRegion');
		       mainRegion.empty();

            App.mainmsg.updateContent(response.responseText,'danger');             

          }); //  $.when(fetchingAuth).done // $when fetchingUser;

    	}) // require
    	
		}

	}


return Controller;

});