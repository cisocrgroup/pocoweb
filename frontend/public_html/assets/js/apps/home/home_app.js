

define(["marionette","app"], function(Marionette,IPS_App){

	var HomeApp = {};

	HomeApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
			"home":"homePortal",
		}
	});


	API = {
		homePortal: function(){
			require(["apps/home/show/show_controller"], function(HomeController){
       				HomeController.showHome();
				});
		},
	};

	IPS_App.on("home:portal",function(){
		IPS_App.navigate("home");
		API.homePortal();
	});

	
	var router = new HomeApp.Router({
		controller: API,
	});


 return HomeApp; 	

});