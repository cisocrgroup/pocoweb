

define(["marionette","app"], function(Marionette,App){

	var HomeApp = {};

	HomeApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
			"home":"homePortal",
    		"":"startsite"
		}
	});


	API = {
		homePortal: function(){
			require(["apps/home/show/show_controller"], function(HomeController){
       				HomeController.showHome();
				});
		},
		startsite: function(){
			require(["apps/home/show/show_controller"], function(HomeController){
    				App.navigate("home");
       				HomeController.showHome();
				});
		}
	
	};

	App.on("home:portal",function(){
		App.navigate("home",{trigger: true});
		API.homePortal();

	});



	var router = new HomeApp.Router({
		controller: API,
	});


 return HomeApp; 	

});