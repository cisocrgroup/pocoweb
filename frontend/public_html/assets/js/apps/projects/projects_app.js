// ===============================
// apps/projects/projects.js
// ===============================

define(["marionette","app"], function(Marionette,App){

	var projectsApp = {};

	projectsApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
		   "projects"    :"listProjects",
  		   "projects/:id"    :"showProject",
  		   "projects/:id/page/:page_id(/line/:line_id)"    :"showPage",
  		   "projects/:id/a_pocoto"    :"showAPoCoTo",
  		   "projects/:id/a_pocoto/lexicon_extension"    :"showLexiconExtension",
  		   "projects/:id/a_pocoto/postcorrection"    :"showPostcorrection",
  		   "projects/:id/a_pocoto/train_postcorrection"    :"showTrainPostcorrection"

		}
	});


	var API = {
		showProject: function(id){
			require(["apps/projects/show/show_controller"], function(ShowController){
       				ShowController.showProject(id);
				});
		},
	
		showPage: function(id,page_id,line_id){
			require(["apps/projects/page/show/show_controller"], function(ShowController){
       				ShowController.showPage(id,page_id,line_id);
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
		},
		showPostcorrection: function(id){
		require(["apps/projects/a_pocoto/protocol/show/show_controller"], function(ShowController){
   				ShowController.showProtocol(id);
			});
		},
		showTrainPostcorrection: function(id){
		require(["apps/projects/a_pocoto/train_postcorrection/show/show_controller"], function(ShowController){
   				ShowController.showTrainPostcorrection(id);
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

	App.on("projects:postcorrection",function(id){
		App.navigate("projects/"+id+"/a_pocoto/postcorrection");
		API.showPostcorrection(id);
	});

	App.on("projects:train_postcorrection",function(id){
		App.navigate("projects/"+id+"/a_pocoto/train_postcorrection");
		API.showTrainPostcorrection(id);
	});

	App.on("projects:show_page",function(id,page_id,line_id){
		if(line_id!=undefined){

		App.navigate("projects/"+id+"/page/"+page_id+"/line/"+line_id);
		} else {
		App.navigate("projects/"+id+"/page/"+page_id);
		line_id=null;
		}
		API.showPage(id,page_id,line_id);
	});

	App.on("projects:list",function(){
		App.navigate("projects");
		API.listProjects();

	});



	var router = new projectsApp.Router({
			controller: API
	});
	

	// router.on("route", function(route, params) {
 //    	App.trigger('page_changed');
	// });


 return projectsApp; 	

});
