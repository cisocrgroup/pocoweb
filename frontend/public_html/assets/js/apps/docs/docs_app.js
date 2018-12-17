// ===============================
// apps/docs/docs_app.js
// ===============================

define(["marionette","app"], function(Marionette,App){

	var DocsApp = {};

	DocsApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
			"docs"                          :"docs"
		
		}
	});

	var API = {
		docs: function(){
			require(["apps/docs/show/show_controller"], function(ShowController){
   				ShowController.showDocs();
			});
		},
		
	};

	App.on("docs:show",function(){
		App.navigate("docs");
		API.docs();
	});

	var router = new DocsApp.Router({
			controller: API
	});
	




 return DocsApp; 	

});
