// ==================================
// apps/about/show/show_controller.js
// ==================================


IPS_App.module("AboutApp.Show", function(Show, IPS_App, Backbone, Marionette, $, _){

 Show.Controller = {

		showAbout: function(){
			var aboutView = new Show.About();
		
			IPS_App.mainRegion.show(aboutView);
		}

	}

});
