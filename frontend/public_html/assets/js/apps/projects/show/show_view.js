// ================================
// apps/projects/show/show_view.js
// ================================

define(["marionette","app","backbone.syphon","common/views","apps/projects/common/views",
        "tpl!apps/projects/show/templates/layout.tpl",
        "tpl!apps/projects/show/templates/info.tpl",
        "tpl!apps/projects/show/templates/resp.tpl"


  ], function(Marionette,App,BackboneSyphon,Views,CommonViews,layoutTpl,infoTpl,respTpl){


    var Show = {};

  Show.Layout = Marionette.View.extend({
    template:layoutTpl,
    regions:{
       headerRegion: "#hl-region"
      ,infoRegion: "#info-region"
      ,respRegion: "#resp-region"
      ,footerRegion: "#footer-region"
    }

  });


  Show.Header = Views.Header.extend({
    initialize: function(){
        this.title = "projects Prototype Tests"
      }
  });



  Show.Info = Marionette.View.extend({
      template: infoTpl,
      events:{
      'click .js-edit-project' : 'edit_clicked',
      'click .js-delete-project' : 'delete_clicked',
      'click .js-add-book' : 'add_book_clicked'

      },
     

      edit_clicked:function(e){
        e.preventDefault();
        this.trigger("show:edit_clicked");
      },

       delete_clicked:function(e){
        e.preventDefault();
        this.trigger("show:delete_clicked");

      },

       add_book_clicked:function(e){
        e.preventDefault();
        this.trigger("show:add_book_clicked");


      },
     onAttach: function(){
       var table = $('#book_table').DataTable();

      }
    
  });



Show.FooterPanel = Views.FooterPanel.extend({
    });

 
	Show.Missingprojects = Views.Error.extend({errortext:"Error 404: projects not found"});

Show.ProjectForm = CommonViews.ProjectForm.extend({
  });

Show.DeleteProjectForm = Views.Confirm.extend({
   confirmClicked: function(e){
     e.preventDefault();
     this.trigger("project:delete_clicked");
   },

  });


return Show;

});

