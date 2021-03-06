// ================================
// apps/projects/a_pocoto/show/show_view.js
// ================================


define(["marionette","app","backbone.syphon","common/views","apps/projects/common/views","apps/projects/page/show/show_view","apps/projects/concordance/show/show_view","apps/projects/a_pocoto/lexicon_extension/show/show_view",
  "apps/projects/a_pocoto/protocol/show/show_view",
        "tpl!apps/projects/show/templates/layout.tpl",
        "tpl!apps/projects/a_pocoto/show/templates/info.tpl"

  ], function(Marionette,App,BackboneSyphon,Views,CommonViews,Page,Concordance,Le,Protocol,layoutTpl,infoTpl){


    var Show = {};

  Show.Layout = Marionette.View.extend({
    template:layoutTpl,
    regions:{
       headerRegion: "#hl-region"
      ,infoRegion: "#info-region"
      ,hubRegion: "#hub-region"
      ,hubRegion2: "#hub2-region"
      ,packagesRegion: "#packages-region"
      ,footerRegion: "#footer-region"
    },
      trackJobStatus : function(){ // regulary check if job is finished
        var that = this;
        this.interval = setInterval(function(){ 
           that.trigger("show:checkJobStatus");
          },

          5000);
      },
      onDestroy : function(){
         clearInterval(this.interval);
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
      'click #js-profile' : 'profile_clicked',
      'click #js-start-le' : 'start_le_clicked'

      }, 
       serializeData: function(){
      return {
        step: Marionette.getOption(this,"step"),
        text: Marionette.getOption(this,"text"),
        color: Marionette.getOption(this,"color"),
        icon: Marionette.getOption(this,"icon"),
        id: Marionette.getOption(this,"id")

        }
      },
     

      profile_clicked:function(e){
        e.preventDefault();
        this.trigger("show:profile_clicked");
      },

       delete_clicked:function(e){
        e.preventDefault();
        this.trigger("show:delete_clicked");

      },

       start_le_clicked:function(e){
        e.preventDefault();
        this.trigger("show:start_le_clicked");


      },
     onAttach: function(){
       var table = $('#book_table').DataTable();

      }
    
  });




Show.FooterPanel = Views.FooterPanel.extend({
    });

 
Show.LexiconExtension = Le.LexiconExtension.extend({

});

Show.Protocol = Protocol.Protocol.extend({

});

Show.Concordance = Concordance.Concordance.extend({

});

Show.AreYouSure = Views.AreYouSure.extend({
      triggers:{
     "click .js-yes":"option:confirm"
    }
  })

Show.OkDialog = Views.OkDialog.extend({
  })


return Show;

});

