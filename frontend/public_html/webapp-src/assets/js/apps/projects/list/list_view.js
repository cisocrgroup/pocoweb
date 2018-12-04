// ================================
// apps/projects/list/list_view.js
// ================================

define(["marionette","app","common/views","apps/projects/common/views",
  "tpl!apps/projects/list/templates/layout.tpl",
  "tpl!apps/projects/list/templates/listpanel.tpl",
], function(Marionette,IPS_App,CommonViews,Views,layoutTpl,panelTpl){

  var List ={};

  List.Layout = Marionette.View.extend({
    template:layoutTpl,
    regions:{
       headerRegion: "#hl-region"
      ,panelRegion: "#panel-region"
      ,infoRegion: "#info-region"
      ,footerRegion: "#footer-region"
    }

  });
  
    List.Header = CommonViews.Header.extend({
    initialize: function(){
        this.title = "OCR Projects"
        this.icon ="fa fa-align-left"
        this.color ="green"
      }
  });


  List.ProjectsList = Views.ProjectsList.extend({
  });

    

  List.Panel = Marionette.View.extend({
    template: panelTpl,
      events:{
      'click .js-create' : 'create_clicked'
      },
      create_clicked: function(){
        this.trigger("list:create_clicked");
      }
    });

  List.FooterPanel = CommonViews.FooterPanel.extend({
    });


 List.ProjectForm = Views.ProjectForm.extend({
  });


return List;

});
