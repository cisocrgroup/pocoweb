
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
			var userShowInfo;
	
			userShowLayout.on("attach",function(){
			  

			  userShowHeader = new Show.Header({model:userModel});
			  userShowPanel = new Show.Panel({model:user});
			  userShowInfo = new Show.Info({model:userModel});
			 
		       userShowLayout.showChildView('headerRegion',userShowHeader);
			   userShowLayout.showChildView('infoRegion',userShowInfo);


			   userShowInfo.on("show:back",function(){
			  	 App.trigger("users:list");
			  });


    		}); // show


			userShowLayout.on("currentuser:loggedOut",function(){
				console.log("loggedOut");
				App.UsersApp.Show.Controller.showUser(id);
	 		}); // on:loggedOut




         App.mainLayout.showChildView('mainRegion',userShowLayout);


    		}).fail(function(response){ 

 			      backdropView.destroy();
				  var errortext = Util.getErrorText(response);    
                  var errorView = new Show.Error({model: currentUser,errortext:errortext})

                  errorView.on("currentuser:loggedIn",function(){
					    App.UsersApp.Show.Controller.showUser(id);
                  });

                  App.mainRegion.show(errorView);  

          }); //  $.when(fetchingAuth).done // $when fetchingUser;

    	}) // require
    	
		}

	}


return Controller;

});