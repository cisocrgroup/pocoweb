// ================================
// apps/projects/show/show_view.js
// ================================

define(["marionette","app","backbone.syphon","common/views","apps/projects/common/views","apps/projects/page/show/show_view","apps/projects/concordance/show/show_view",
        "tpl!apps/projects/show/templates/layout.tpl",
        "tpl!apps/projects/show/templates/info.tpl",
        "tpl!apps/projects/show/templates/split.tpl"

  ], function(Marionette,App,BackboneSyphon,Views,CommonViews,Page,Concordance,layoutTpl,infoTpl,splitTpl){


    var Show = {};

  Show.Layout = Marionette.View.extend({
    template:layoutTpl,
    regions:{
       headerRegion: "#hl-region"
      ,infoRegion: "#info-region"
      ,hubRegion: "#hub-region"
      ,hubRegion2: "#hub2-region"

      ,footerRegion: "#footer-region"
    }

  });


  Show.Header = Views.Header.extend({
    initialize: function(){
        this.title = "projects Prototype Tests"
      }
  });


 Show.Hub = Views.CardHub.extend({
 })

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

Show.Page = Page.Page.extend({});
Show.Concordance = Concordance.Concordance.extend({});

Show.FooterPanel = Views.FooterPanel.extend({
    });

 
	Show.Missingprojects = Views.Error.extend({errortext:"Error 404: projects not found"});

Show.ProjectForm = CommonViews.ProjectForm.extend({
  });

Show.AreYouSure = Views.AreYouSure.extend({
      triggers:{
     "click .js-yes":"delete:confirm"
    }
  })

Show.OkDialog = Views.OkDialog.extend({
  })

Show.Split = Marionette.View.extend({
    template: splitTpl,
    events:{
     "click .js-confirm":"confirmClicked",
    },
  serializeData: function(){
      return {
        title: Marionette.getOption(this,"title"),
        text: Marionette.getOption(this,"text"),
        id: Marionette.getOption(this,"id"),
        n: Marionette.getOption(this,"n")

      }
  },
  confirmClicked: function(){
    var n = $("#split-n").val();
    var random = false;
     if($("#checkRnd").is(":checked")) {
        random=true;
      }
    var data={n:n,random:random};
    this.trigger("split:confirmed",data);
  },

   onAttach: function(){

    this.$el.addClass("modal fade");
    this.$el.attr("tabindex","-1");
    this.$el.attr("role","dialog");
    this.$el.attr("id","splitModal");

    $("#splitModal").modal();
    var that = this;

    $("#split-n").on('input',function(){
      var n = $(this).val();
      $('#splitLabel').text("Split into " + n + " pages");
    })

     $('#checkRnd').change(function() {
        if($(this).is(":checked")) {
          $("#split-n").attr('disabled',true);
          $('#splitLabel').text("Split randomly");
        }
        else{
          $("#split-n").attr('disabled',false);
          var n = $("#split-n").val();
          $('#splitLabel').text("Split into " + n + " pages");
       }        
    });


    }
     
  });

return Show;

});

