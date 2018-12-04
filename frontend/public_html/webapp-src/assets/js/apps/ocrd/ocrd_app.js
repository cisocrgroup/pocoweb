// ===============================
// apps/ocr/ocr.js
// ===============================

define(["marionette","app"], function(Marionette,App){

	var OcrdApp = {};

	OcrdApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
		   "ocr"    :"showOcr"
		}
	});

	var API = {
	
	
		showOcr: function(id){
			require(["apps/ocrd/show/show_controller"], function(ShowController){
       				ShowController.showOcrd(id);
				});
		}
	
	};


	App.on("ocrd:show",function(id){
		App.navigate("ocr");
		API.showOcr(id);
	});


	var router = new OcrdApp.Router({
			controller: API
	});
	


 return OcrdApp; 	

});
