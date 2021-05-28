// ===============================
// apps/docs/docs_app.js
// ===============================

define(["marionette","app"], function(Marionette,App){

	var DocsApp = {};

	DocsApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
			"documentation"                          :"docsPortal",
			"documentation/api"                          :"docsAPI",
			"documentation/manual"                          :"docsManual",
			"documentation/legalnotice"                          :"docsLegal"

		},
		
	});

	var API = {
		docsPortal: function(){
			require(["apps/docs/home/home_controller"], function(HomeController){
   				HomeController.showHome();
			});
		},
		docsAPI: function(){
			require(["apps/docs/show/show_controller"], function(ShowController){
   				ShowController.showDocs("api_doc");
			});
		},
		docsManual: function(){
			require(["apps/docs/show/show_controller"], function(ShowController){
   				ShowController.showDocs("manual");
			});
		},
		docsLegal: function(){
			require(["apps/docs/show/show_controller"], function(ShowController){
   				ShowController.showDocs("legal_notice");
			});
		},
	};

	App.on("docs:home",function(){
		App.navigate("documentation");
		API.docsPortal();
	});

	App.on("docs:api",function(){
		App.navigate("documentation/api");
		API.docsAPI();
	});

	App.on("docs:manual",function(){
		App.navigate("documentation/manual");
		API.docsManual();
	});

	App.on("docs:legal",function(){
		App.navigate("documentation/legalnotice");
		API.docsLegal();
	});

	var router = new DocsApp.Router({
			controller: API
	});
	




 return DocsApp; 	

});
