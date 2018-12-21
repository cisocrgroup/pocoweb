
define(["app","common/views",
        "tpl!apps/users/show/templates/layout.tpl",
        "tpl!apps/users/show/templates/panel.tpl",
        "tpl!apps/users/show/templates/info.tpl"

  ], function(ResearchTool,views,layoutTpl,panelTpl,infoTpl){


ResearchTool.module("UsersApp.Show", function(Show, ResearchTool, Backbone, Marionette, $, _){


  Show.Layout = ResearchTool.Common.Views.LoginUserLayout.extend({
    template:layoutTpl,
    regions:{
      headerRegion: "#hl-region",
      panelRegion: "#panel-region",
      infoRegion: "#info-region"
    }

  });


  Show.Header = ResearchTool.Common.Views.Header.extend({
    initialize: function(){
        this.title = "User Account: "+this.model.get('username');

        this.breadcrumbs = [
        {name: "Users", url: "#/users"},
        {name: this.model.get('username'), url: "#/users/"+this.model.get('user_id'),current:"true"},
        ]
      }
  });



  Show.Info = Marionette.ItemView.extend({
      template: infoTpl,
      className: "row bg_style",
      events:{
        "click button.js-back":   "backClicked",
      },
      backClicked: function(e){
      e.preventDefault();
      this.trigger("show:back");
     }, 
     onShow: function(){
         $('#log_table').dataTable( {
              "bAutoWidth": false,
               "bStateSave": true,
                 "lengthMenu": [[10, 25, 50, -1], [10, 25, 50, "All"]],
               "iDisplayLength": 10
            });
      }          

  });



  Show.Panel = Marionette.ItemView.extend({
      template: panelTpl,
      className: "row"         
  });


 Show.Error = ResearchTool.Common.Views.LoginError.extend({});


	Show.MissingUser = ResearchTool.Common.Views.Error.extend({errortext:"Error 404: User not found"});


});

return ResearchTool.UsersApp.Home;

});

