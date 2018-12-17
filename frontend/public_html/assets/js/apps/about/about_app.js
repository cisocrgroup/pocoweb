
define(["app"], function(IPS_App){

IPS_App.module("AboutApp", function(AboutApp,IPS_App,Backbone,Marionette,$,_){

	AboutApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
			"about":"aboutPortal",
     		"about/project":"aboutProject",
			// "about/team":"aboutTeam"

		}
	});

	var API = {
		
		aboutPortal: function(){
			require(["apps/about/home/home_controller"], function(HomeController){
       				HomeController.showHome();
				});
		},
		aboutProject: function(){
			require(["apps/about/project/project_controller"], function(ProjectController){
       				ProjectController.showProject();
				});
		},
		aboutTeam: function(){
		require(["apps/about/team/team_controller"], function(TeamController){
   				TeamController.showTeam();
			});
	},

	};


	IPS_App.on("about:show",function(){
	 	IPS_App.navigate("about");
	API.showAbout;
	});



	IPS_App.on("about:legalnotice",function(){
	 	IPS_App.navigate("about/legalnotice");
	API.aboutLegalNotice();
	});



	IPS_App.addInitializer(function(){
		new AboutApp.Router({
			controller: API
		});
	});
});


 return IPS_App.AboutApp; 	

});
