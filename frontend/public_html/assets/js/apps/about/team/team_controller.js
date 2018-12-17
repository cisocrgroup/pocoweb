// ==================================
// apps/about/team/team_controller.js
// ==================================

define(["app","apps/about/team/team_view"], function(IPS_App){

IPS_App.module("AboutApp.Team", function(Team, IPS_App, Backbone, Marionette, $, _){

 Team.Controller = {

 	showTeam: function(){

   		var backdropView = new IPS_App.Common.Views.LoadingBackdropOpc();
   		IPS_App.backdropRegion.show(backdropView);

		var currentUser = IPS_App.request('app:currentUser');

    	$.when(currentUser).done(function(currentUser){


		var aboutTeamLayout = new Team.Layout();
		var aboutTeamHeader = new Team.Header();
		var aboutTeamHub = new Team.Hub({model:currentUser});
	
	    var iconLoaders= aboutTeamHub.preloadIcons();
	    var imgLoaders = aboutTeamHeader.preloadBackgrounds();
		var allLoaders = imgLoaders.concat(iconLoaders);

       $.when.apply($,allLoaders).done(function() {
       backdropView.destroy();

		aboutTeamLayout.on("show",function(){
			 $(document).foundation('reflow');
				aboutTeamLayout.headerRegion.show(aboutTeamHeader);
				aboutTeamLayout.contentRegion.show(aboutTeamHub);
			
 		}); // on:show

		IPS_App.mainRegion.show(aboutTeamLayout);

		}); // apply
	}); // $when

	}
 }
});

return IPS_App.AboutApp.Team.Controller;

});
