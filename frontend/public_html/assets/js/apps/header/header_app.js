
define(["marionette","app"], function(Marionette,App){

	var HeaderApp={};
		HeaderApp.Router = Marionette.AppRouter.extend({
		appRoutes: {
		}
	});
		HeaderApp.API = {
			showHeader: function(callback){
				require(["apps/header/show/show_controller"], function(ShowController){
					ShowController.showHeader();
					callback();
					
				});
			}
		};
	
  return HeaderApp; 	

});