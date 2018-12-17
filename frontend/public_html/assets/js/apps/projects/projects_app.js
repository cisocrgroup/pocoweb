// ===============================
// apps/projects/projects.js
// ===============================

define(["marionette","app"], function(Marionette,App){

	var projectsdApp = {};

	projectsdApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
		   "projects/list"    :"listProjects",
  		   "projects/:id"    :"showProject"

		}
	});

	var API = {
	
	
		showProject: function(id){
			require(["apps/projects/show/show_controller"], function(ShowController){
       				ShowController.showProject(id);
				});
		},

		listProjects: function(){
			require(["apps/projects/list/list_controller"], function(ListController){
       				ListController.listProjects();
				});
		}
	
	};


	App.on("projects:show",function(id){
		App.navigate("projects");
		API.showProject(id);
	});


	App.on("projects:list",function(){
		App.navigate("projects/list");
		API.listProjects();
	});



	var router = new projectsdApp.Router({
			controller: API
	});
	


 return projectsdApp; 	

});
