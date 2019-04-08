
define(["marionette","app"], function(Marionette,IPS_App){

	var HeaderApp={};

		HeaderApp.API = {
			showHeader: function(callback){
				require(["apps/header/show/show_controller"], function(ShowController){
					ShowController.showHeader();
					callback();
					
				});
			}
		};

	// HeaderApp.on("start", function(){
	// 	API.showHeader();
	// });	


  return HeaderApp; 	

});