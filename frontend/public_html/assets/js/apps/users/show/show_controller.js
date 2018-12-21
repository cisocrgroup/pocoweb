
define(["app","common/util","apps/users/show/show_view"], function(App,Util){


 var Controller = {

		showUser: function(id){

	   		require(["entities/users"], function(){

	   		var backdropView = new App.Common.Views.LoadingBackdropOpc();
	   		App.backdropRegion.show(backdropView);

			var currentUser = App.request('app:currentUser');
			var fetchingUser= App.request("user:entity",id);

			$.when(fetchingUser).done(function(user){
			backdropView.destroy();

		 	//currentUser.set({"url_id":id}); // pass url_id to view..
			var userShowLayout = new Show.Layout({model:currentUser,preventDestroy:true});

			var userShowHeader;
     		var userShowPanel;
			var userShowInfo;
	
			userShowLayout.on("show",function(){
			  

			  userShowHeader = new Show.Header({model:user,preventDestroy:true});
			  userShowPanel = new Show.Panel({model:user,preventDestroy:true});
			  userShowInfo = new Show.Info({model:user,preventDestroy:true});
			 
		      userShowLayout.headerRegion.show(userShowHeader);
			  userShowLayout.panelRegion.show(userShowPanel);
			  userShowLayout.infoRegion.show(userShowInfo);

			   userShowInfo.on("show:back",function(){
			  	 App.trigger("users:list");
			  });


    		}); // show


			userShowLayout.on("currentuser:loggedOut",function(){
				console.log("loggedOut");
				App.UsersApp.Show.Controller.showUser(id);
	 		}); // on:loggedOut




			App.mainRegion.show(userShowLayout);


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