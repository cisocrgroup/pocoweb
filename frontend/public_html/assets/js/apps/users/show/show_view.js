
define(["app","marionette","common/views",
        "tpl!apps/users/show/templates/layout.tpl",
        "tpl!apps/users/show/templates/panel.tpl",
        "tpl!apps/users/show/templates/info.tpl"

  ], function(App,Marionette,Views,layoutTpl,panelTpl,infoTpl){


var Show = {};

  Show.Layout = Marionette.View.extend({
    template:layoutTpl,
    regions:{
      headerRegion: "#hl-region",
      panelRegion: "#panel-region",
      infoRegion: "#info-region"
    }

  });


  Show.Header = Views.Header.extend({
    initialize: function(){
      this.color = "red";
      this.icon = "fa fa-user-circle";
        this.title = 'Account for user #'+this.model.get('id');
        if(this.model.get('admin')) this.title += " (Administrator)"
      }
  });



  Show.Info = Marionette.View.extend({
      template: infoTpl,
      events:{
        "click button.js-back":   "backClicked",
      },
      backClicked: function(e){
      e.preventDefault();
      this.trigger("show:back");
     },     

  });



  Show.Panel = Marionette.View.extend({
      template: panelTpl,
      className: "row"         
  });






return Show;

});

