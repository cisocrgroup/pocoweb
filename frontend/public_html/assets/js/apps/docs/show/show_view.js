// ================================
// apps/docs/show/show_view.js
// ================================

define(["marionette","app","common/views",
        "tpl!apps/docs/show/templates/layout.tpl",
        "tpl!apps/docs/show/templates/info.tpl"


  ], function(Marionette,IPS_App,Views,layoutTpl,infoTpl){


    var Show = {};

  Show.Layout = Marionette.View.extend({
    template:layoutTpl,
    regions:{
       headerRegion: "#hl-region"
      ,infoRegion: "#info-region"
      ,buttonPanelRegion: "#button-panel-region"
      ,structuresRegion: "#structures-region"
      ,panelRegion: "#panel-region"
    }

  });


  Show.Header = Views.Header.extend({
    initialize: function(){
        this.title = "Api documentation"

      }
  });



  Show.Info = Marionette.View.extend({
      template: infoTpl,
      className: "",
      onAttach: function(){

         var content =  Marionette.getOption(this,"data");
         $('#docs').append(content);
         this.trigger('doc:append');
      }

  });




  Show.FooterPanel = Views.FooterPanel.extend({
  });



	Show.Missingindexstructure = Views.Error.extend({errortext:"Error 404: indexstructure not found"});


return Show;

});
