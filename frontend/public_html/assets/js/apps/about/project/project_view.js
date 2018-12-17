// ==================================
// apps/about/project/project_view.js
// ==================================

define(["app","common/views",
        "tpl!apps/about/project/templates/aboutproject.tpl"

  ], function(IPS_App,views,aboutTpl){

IPS_App.module("AboutApp.Project", function(Project, IPS_App, Backbone, Marionette, $, _){

		Project.About = IPS_App.Common.Views.Layout.extend({
		});

		Project.Content = Marionette.ItemView.extend({
		template: aboutTpl, 
    className:"row bg_style"
		});


	  Project.Header = IPS_App.Common.Views.Header.extend({
       initialize: function(){
        this.title = "About the project"
      }
  });
});
return IPS_App.AboutApp.Project;

});

