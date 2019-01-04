// ===============================
// apps/projects/projects.js
// ===============================

define(["marionette","app"], function(Marionette,App){

	var projectsdApp = {};

	projectsdApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
		   "projects"    :"listProjects",
  		   "projects/:id/page/:page_id"    :"showProject"

		}
	});

	var API = {
	
	
		showProject: function(id,page_id){
			require(["apps/projects/show/show_controller"], function(ShowController){
       				ShowController.showProject(id,page_id);
				});
		},

		listProjects: function(){
			require(["apps/projects/list/list_controller"], function(ListController){
       				ListController.listProjects();
				});
		}
	
	};


	App.on("projects:show",function(id,page_id){
		App.navigate("projects/:id/page/:page_id");
		API.showProject(id,page_id);
	});


	App.on("projects:list",function(){
		App.navigate("projects");
		API.listProjects();
	});



	var router = new projectsdApp.Router({
			controller: API
	});
	


 return projectsdApp; 	

});
