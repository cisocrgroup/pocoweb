
define(["app","apps/users/common/views"], function(ResearchTool){

ResearchTool.module("UsersApp.New", function(New,ResearchTool,Backbone,Marionette,$,_){

		New.User = ResearchTool.UsersApp.Common.Views.Form.extend({
			
			initialize: function(){
		 	this.title = "Create New Account";
		 	}
		});

});

return ResearchTool.UsersApp.New.User;

});
