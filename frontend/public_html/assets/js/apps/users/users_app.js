
define(["marionette","app"], function(Marionette,App){

	var UsersApp = {};

	UsersApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
			"users":"usersPortal",
			"users/browse":"listUsers",
			"users/login":"login",
    		"users/newUser":"newUser",
    		"users/:id":"showUser",
			"users/:id/edit":"editUser"
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
		newUser: function(){
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
		}
	};

	App.on("users:home",function(){
		App.navigate("users");
		API.usersPortal();
	});

	App.on("users:list",function(){
		App.navigate("users/browse");
		API.listUsers();
	});


	App.on("users:list",function(){
		App.navigate("users/login");
		API.login();
	});

	App.on("user:show",function(id){
	 	App.navigate("users/"+ id);
	API.showUser(id);
	});

	App.on("user:edit",function(id){
	 	App.navigate("users/"+ id+"/edit");
	API.editUser(id);
	});


	var router = new UsersApp.Router({
		controller: API,
	});
 return UsersApp; 	

});