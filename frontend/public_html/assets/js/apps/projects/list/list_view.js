// ================================
// apps/projects/list/list_view.js
// ================================

define(["marionette","app","common/views","apps/projects/common/views",
  "tpl!apps/projects/list/templates/layout.tpl",
  "tpl!apps/projects/list/templates/listpanel.tpl"
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
        this.title = "Projects"
        this.icon ="fas fa-list"
        this.color ="green"
      }
  });


  List.ProjectsList = Views.ProjectsList.extend({
  });



  List.Panel = Marionette.View.extend({
    template: panelTpl,
      events:{
      'click .js-create' : 'create_clicked',
      'click .js-pool' : 'pool_clicked',
      'click .js-own' : 'own_clicked'

      },
      create_clicked: function(){
        this.trigger("list:create_clicked");
      },
      pool_clicked: function(){
        this.trigger("list:pool_clicked");
      },
      own_clicked: function(){
        this.trigger("list:own_clicked");
      },
	serializeData: function(){
		return {
			user: Marionette.getOption(this,"user"), 
      auth: Marionette.getOption(this,"auth")

		}
	}
    });

  List.FooterPanel = CommonViews.FooterPanel.extend({manual:true,title: "Back to Home <i class='fas fa-home'></i>"
    });


 List.ProjectForm = Views.ProjectForm.extend({
  });



return List;

});
