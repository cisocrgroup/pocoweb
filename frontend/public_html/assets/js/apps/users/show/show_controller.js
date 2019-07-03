
define(["app","common/util","apps/users/show/show_view"], function(App,Util,Show){


 var Controller = {

		showUser: function(id){

	   		require(["entities/users"], function(UserEntities){

	   		// var backdropView = new App.Common.Views.LoadingBackdropOpc();
	   		// App.backdropRegion.show(backdropView);


			var fetchingUser = UserEntities.API.getUser(id);

			$.when(fetchingUser).done(function(user){
			// backdropView.destroy();

			var userModel = new UserEntities.User(user);
			console.log(userModel)

			var userShowLayout = new Show.Layout();

			var userShowHeader;
     		var userShowPanel;
			var userShowForm;
	
			userShowLayout.on("attach",function(){
			  

			  userShowHeader = new Show.Header({model:userModel});
			  userShowPanel = new Show.Panel({model:user});
			  userShowForm = new Show.Form({model:userModel});
			  userShowFooter = new Show.FooterPanel();

		       userShowLayout.showChildView('headerRegion',userShowHeader);
   		       userShowLayout.showChildView('panelRegion',userShowPanel);
			   userShowLayout.showChildView('infoRegion',userShowForm);
			   userShowLayout.showChildView('footerRegion',userShowFooter);


			   userShowPanel.on("show:back",function(){
			  	 App.trigger("users:list");
			  });

			  userShowPanel.on("show:update",function(data){
			  		data['id'] = user['id'];
			  		var updatingUser = UserEntities.API.updateUser(data);
						$.when(updatingUser).done(function(user){
							   App.mainmsg.updateContent("Account updated successfully.",'success');      
							   $('.loginname').text(user.name);       

						}).fail(function(response){ 

			            App.mainmsg.updateContent(response.responseText,'warning');             

			          });

			  });
 			userShowPanel.on("show:delete",function(){

 				var confirmModal = new Show.AreYouSure({title:"Are you sure...",text:"...you want to delete your account?",id:"deleteModal"})
 				App.mainLayout.showChildView('dialogRegion',confirmModal)
			  });


    		}); // show





         App.mainLayout.showChildView('mainRegion',userShowLayout);


    		}).fail(function(response){ 

			 Util.defaultErrorHandling(response,'danger');                        
          

          }); //  $.when(fetchingAuth).done // $when fetchingUser;

    	}) // require
    	
		}

	}


return Controller;

});