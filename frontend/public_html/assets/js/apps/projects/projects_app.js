// ===============================
// apps/projects/projects.js
// ===============================

define(["marionette","app"], function(Marionette,App){

	var projectsdApp = {};

	projectsdApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
		   "projects"    :"listProjects",
  		   "projects/:id"    :"showProject",
  		   "projects/:id/page/:page_id"    :"showPage",
  		   "projects/:id/a_pocoto"    :"showAPoCoTo",
  		   "projects/:id/a_pocoto/lexicon_extension"    :"showLexiconExtension"

		}
	});

	var API = {
		showProject: function(id){
			require(["apps/projects/show/show_controller"], function(ShowController){
       				ShowController.showProject(id);
				});
		},
	
		showPage: function(id,page_id){
			require(["apps/projects/page/show/show_controller"], function(ShowController){
       				ShowController.showPage(id,page_id);
				});
		},

		listProjects: function(){
			require(["apps/projects/list/list_controller"], function(ListController){
       				ListController.listProjects();
				});
		},
		showAPoCoTo: function(id){
			require(["apps/projects/a_pocoto/show/show_controller"], function(ShowController){
       				ShowController.showAPoCoTo(id);
				});
		},
		showLexiconExtension: function(id){
			require(["apps/projects/a_pocoto/lexicon_extension/show/show_controller"], function(ShowController){
       				ShowController.showLexiconExtension(id);
				});
		}
	
	};


	App.on("projects:show",function(id){
		App.navigate("projects/"+id);
		API.showProject(id);
	});

	App.on("projects:a_pocoto",function(id){
		App.navigate("projects/"+id+"/a_pocoto");
		API.showAPoCoTo(id);
	});

	App.on("projects:lexicon_extension",function(id){
		App.navigate("projects/"+id+"/a_pocoto/lexicon_extension");
		API.showLexiconExtension(id);
	});

	App.on("projects:show_page",function(id,page_id){
		App.navigate("projects/"+id+"/page/"+page_id);
		API.showPage(id,page_id);
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
