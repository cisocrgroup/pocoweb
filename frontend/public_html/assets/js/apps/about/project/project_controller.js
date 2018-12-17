// ========================================
// apps/about/project/project_controller.js
// ========================================

define(["app","apps/about/project/project_view"], function(IPS_App){

IPS_App.module("AboutApp.Project", function(Project, IPS_App, Backbone, Marionette, $, _){

 Project.Controller = {

		showProject: function(){
			var aboutProjectLayout = new Project.About();
			var aboutProjectHeader = new Project.Header();
			var aboutProjectContent = new Project.Content();

			aboutProjectLayout.on('show',function(){
				aboutProjectLayout.headerRegion.show(aboutProjectHeader);
    			aboutProjectLayout.contentRegion.show(aboutProjectContent);

			});


			IPS_App.mainRegion.show(aboutProjectLayout);
		}

	}

});
return IPS_App.AboutApp.Project.Controller;

});
