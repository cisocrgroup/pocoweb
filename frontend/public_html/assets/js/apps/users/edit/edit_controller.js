
define(["app","common/util","apps/users/edit/edit_view"], function(App,Util){



var Controller = {

		editUser: function(id){

     		require(["entities/users"], function(){

	   		var backdropView = new App.Common.Views.LoadingBackdropOpc();
	   		App.backdropRegion.show(backdropView);

			var currentUser = App.request('app:currentUser');
			var roles = App.request("user_role:entities");
			var fetchingUser= App.request("user:entity",id);

			$.when(fetchingUser,currentUser).done(function(user,currentuser){
			backdropView.destroy();
		 //	currentUser.set({"url_id":id}); // pass url_id to view..
			var userEditLayout = new Edit.Layout({model:currentUser,preventDestroy:true});
    		userEditLayout.on("show",function(){
			 var userEditHeader = new Edit.Header({model:user,preventDestroy:true});
			 var userEditForm = new Edit.User({model:user,roles:roles,role_idx:currentUser.get('role_idx'),preventDestroy:true});
			  
				userEditLayout.headerRegion.show(userEditHeader);
    			userEditLayout.contentRegion.show(userEditForm);

    			  userEditForm.on("form:submit",function(data){

	    			  	data['password']="xyz";
	    			  	data['confirmPassword']="xyz"; // workaround for PW equalTo valid
	    			  	data['securitycode']="xyz"; // workaround for PW equalTo valid

	    			  	data['currentuser_id']=currentUser.get('user_id'); // who made the change
	    			    user.set(data);	
			        if(user.save(data)){

			          	if(currentUser.get('user_id')==id){ currentUser.set(data), currentUser.set({'loggedIn':true});}
           			 	App.trigger("user:show",id);
       			   	    Util.writeToMsgConsole("Edit Account: successful","success"); // write success msg

			           }
			           else{
       			   	    Util.writeToMsgConsole("Edit Account: failed","error"); // write success msg
			           }

			    });

    			 userEditForm.on("form:back",function(data){
    			 	App.trigger("user:show",id);
    			 });

    		}); // show
    	
				userEditLayout.on("currentuser:loggedOut",function(){
					App.UsersApp.Edit.Controller.editUser(id);
		 		}); // on:loggedOut
	

				App.mainRegion.show(userEditLayout);


		 }).fail(function(response){ 

 			      backdropView.destroy();
				  var errortext = Util.getErrorText(response);    
                  var errorView = new Edit.Error({model: currentUser,errortext:errortext})

                  errorView.on("currentuser:loggedIn",function(){
					    App.UsersApp.Edit.Controller.editUser(id);
                  });

                  App.mainRegion.show(errorView);  

          }); //  $.when(fetchingAuth).done // $when fetchingUsers;

    	});// require

		}

	}


return Controller;

});