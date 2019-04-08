// ============================
// apps/about/show/show_view.js
// ============================

IPS_App.module("AboutApp.Show", function(Show, IPS_App, Backbone, Marionette, $, _){

	Show.About = Marionette.ItemView.extend({
		template: "#about-view",
	  
		});
});
