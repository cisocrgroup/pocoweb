
define(["app","common/util","apps/users/show/show_view","apps/users/logs/logs_view"], function(App,Util,Show,Logs){


 var Controller = {

		showUser: function(id){

	   		require(["entities/users"], function(UserEntities){

	   		// var backdropView = new App.Common.Views.LoadingBackdropOpc();
	   		// App.backdropRegion.show(backdropView);


			var fetchingUser = UserEntities.API.getUser(id);
			var fetchingLogs = App.getLastMessages(App.getCurrentUser()['id']);

			$.when(fetchingUser,fetchingLogs).done(function(user,logs){
			// backdropView.destroy();


			var userModel = new UserEntities.User(user);
			console.log(userModel)

			var userShowLayout = new Show.Layout();

			var userShowHeader;
     		var userShowPanel;
			var userShowForm;
			var userLogs;

			userShowLayout.on("attach",function(){

			  var breadcrumbs = [
             {title:"<i class='fas fa-home'></i>",url:"/"},
             {title:"User Management",url:"#users"},
             {title:"My Account",url:""},
       		  ];

			  userShowHeader = new Show.Header({breadcrumbs: breadcrumbs,model:userModel});
			  userShowPanel = new Show.Panel({model:user});
			  userShowForm = new Show.Form({model:userModel});
			  userLogs = new Logs.List({collection: logs,table_id:"log_list_user"});

			  userShowFooter = new Show.FooterPanel({title: "Back to User Management <i class='fas fa-users-cog'></i>",manual:true});


     
		       userShowLayout.showChildView('headerRegion',userShowHeader);
   		       userShowLayout.showChildView('panelRegion',userShowPanel);
			   userShowLayout.showChildView('infoRegion',userShowForm);
			   userShowLayout.showChildView('logsRegion',userLogs);

			   userShowLayout.showChildView('footerRegion',userShowFooter);


			   userShowFooter.on("go:back",function(){
			  	 App.trigger("users:home");
			  });

			  userShowPanel.on("show:update",function(data){
			  		data['id'] = user['id'];
			  		var updatingUser = UserEntities.API.updateUser(data);
						$.when(updatingUser).done(function(user){
						  App.mainmsg.updateContent("Account updated successfully.",'success',true,user.request_url);
                          console.log(user);
                          App.setCurrentUser(user);
						  $('.loginname').text(user.name);
						}).fail(function(response){
			            App.mainmsg.updateContent(response.responseText,'warning',true,response.request_url);
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