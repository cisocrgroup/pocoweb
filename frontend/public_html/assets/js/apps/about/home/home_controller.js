// ==================================
// apps/about/home/home_controller.js
// ==================================

define(["app","apps/about/home/home_view"], function(IPS_App){

IPS_App.module("AboutApp.Home", function(Home, IPS_App, Backbone, Marionette, $, _){

 Home.Controller = {

 	showHome: function(){

   		var backdropView = new IPS_App.Common.Views.LoadingBackdropOpc();
   		IPS_App.backdropRegion.show(backdropView);

		var currentUser = IPS_App.request('app:currentUser');

    	$.when(currentUser).done(function(currentUser){


		var aboutHomeLayout = new Home.Layout();
		var aboutHomeHeader = new Home.Header();
		var aboutHomeHub = new Home.Hub({model:currentUser});
	
	    var iconLoaders= aboutHomeHub.preloadIcons();
	    var imgLoaders = aboutHomeHeader.preloadBackgrounds();
		var allLoaders = imgLoaders.concat(iconLoaders);

       $.when.apply($,allLoaders).done(function() {
       backdropView.destroy();

		aboutHomeLayout.on("show",function(){
			 $(document).foundation('reflow');
				aboutHomeLayout.headerRegion.show(aboutHomeHeader);
				aboutHomeLayout.contentRegion.show(aboutHomeHub);
			
 		}); // on:show


		aboutHomeHub.on("contact:show",function(){

		    	var contactView = new Home.Contact({asModal:true});
		     	IPS_App.dialogRegion.show(contactView);

	   		 });



		IPS_App.mainRegion.show(aboutHomeLayout);

		}); // apply
	}); // $when

	}
 }
});

return IPS_App.AboutApp.Home.Controller;

});
