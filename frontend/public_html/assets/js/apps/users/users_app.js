
define(["marionette","app"], function(Marionette,App){

	var UsersApp = {};

	UsersApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
			"users":"usersPortal",
			"users/list":"listUsers",
			"users/login":"login",
    		"users/new":"newUser",
    		"users/:id":"showUser",
			"users/:id/edit":"editUser",
			"users/account":"showUser"

		}
	});

	var API = {
		usersPortal: function(){
			require(["apps/users/home/home_controller"], function(HomeController){
       				HomeController.showHome();
			});
		},
		listUsers: function(){
			require(["apps/users/list/list_controller"], function(ListController){
   				ListController.listUsers();
			});
		},
		login: function(){
			require(["apps/users/login/login_controller"], function(LoginController){
       				LoginController.showLogin();
				});
		},
		showUser: function(id){
			require(["apps/users/show/show_controller"], function(ShowController){
       				ShowController.showUser(id);
				});
		},
		editUser: function(id){
			require(["apps/users/edit/edit_controller"], function(EditController){
       				EditController.editUser(id);
				});
		},
		newUser: function(id){
			require(["apps/users/new/new_controller"], function(NewController){
       				NewController.newUser(id);
				});
		}
	};

	App.on("users:home",function(){
		App.navigate("users");
		API.usersPortal();
	});

	App.on("users:list",function(){
		App.navigate("users/list");
		API.listUsers();
	});



	App.on("users:show",function(id){
	 	App.navigate("users/"+ id);
	API.showUser(id);
	});

	App.on("users:edit",function(id){
	 	App.navigate("users/"+ id+"/edit");
	API.editUser(id);
	});

	App.on("users:account",function(id){
	 	App.navigate("users/account");
	API.showUser(id);
	});

	App.on("users:new",function(id){
	 	App.navigate("users/new");
	API.newUser(id);
	});

	var router = new UsersApp.Router({
		controller: API,
	});

	// router.on("route", function(route, params) {
 //    	App.trigger('router:page_changed');
	// });


 return UsersApp; 	

});