
define(["app","marionette","apps/users/common/views","common/views",
        "tpl!apps/users/show/templates/layout.tpl",
        "tpl!apps/users/show/templates/panel.tpl"

  ], function(App,Marionette,UserViews,Views,layoutTpl,panelTpl){


var Show = {};

  Show.Layout = Marionette.View.extend({
    template:layoutTpl,
    regions:{
      headerRegion: "#hl-region",
      panelRegion: "#panel-region",
      infoRegion: "#info-region",
      footerRegion: "#footer-region"

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



  Show.Form = UserViews.Form.extend({

  });


  Show.AreYouSure = Views.AreYouSure.extend({})

  Show.Panel = Marionette.View.extend({
      template: panelTpl,
       events:{
        "click button.js-back":   "backClicked",
        "click button.js-update": "updateClicked",
        "click button.js-delete": "deleteClicked"
      },
      backClicked: function(e){
      e.preventDefault();
      this.trigger("show:back");
     },     
      updateClicked: function(e){
      e.preventDefault();
      var data = {}
      data['name'] = $("input[name=name]").val();
      data['email'] = $("input[name=email]").val();
      data['institute'] = $("input[name=institute").val();
      data['password'] = $("input[name=password]").val();
      data['new_password'] = $("input[name=new_password]").val();
          console.log(data)
      this.trigger("show:update",data);
     },
      deleteClicked: function(e){
      e.preventDefault();
      this.trigger("show:delete");
     }     
  });



Show.FooterPanel = Views.FooterPanel.extend({
    });


return Show;

});

