// ================================
// apps/projects/a_pocoto/protocol/show/show_view.js
// ================================

define(["marionette","app","jquery-ui","backbone.syphon","common/views","apps/projects/page/show/show_view","apps/projects/concordance/show/show_view",
        "tpl!apps/projects/a_pocoto/protocol/show/templates/info.tpl"
  ], function(Marionette,App,jquery_ui,BackboneSyphon,Views,Page,Concordance,infoTpl){


    var Show = {};

  Show.Layout = Views.Layout.extend({
       trackJobStatus : function(){ // regulary check if job is finished
        var that = this;
        this.interval = setInterval(function(){
           that.trigger("show:checkJobStatus");
          },
          5000);
      },
      stopJobTracking : function(){
         clearInterval(this.interval);
      },
      onDestroy : function(){
         clearInterval(this.interval);
      }
  });


  Show.Header = Views.Header.extend({
    initialize: function(){
      }
  });



  Show.Protocol = Marionette.View.extend({
      template: infoTpl,
      events:{
      'click tbody tr' : 'row_clicked',
      'click .js-pr-redo' : 'pr_redo_clicked'
      },
    //   serializeData: function(){

    //      var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
    //      console.log(data)
    //     return data;

    // },
    serializeData: function(){
      return {
        pr: Marionette.getOption(this,"pr")
      }
    },

    pr_redo_clicked:function(e){
        e.preventDefault();

        this.trigger("show:pr_redo_clicked");
      },
      row_clicked:function(e){
        e.preventDefault();
        var word = $($(e.currentTarget).find("td")[0]).text();

        this.trigger("show:word_clicked",word);
      },


     onAttach: function(){
      var always_table = $('#always').DataTable({
                "scrollY": '556px',
                "info":false,
                "paging": false,
                "lengthChange": false,
                "order": [[ 0, "asc" ]]

           });

           this.always_table = always_table;

          $('#always_filter').parent().prev().remove();
          $('#always_filter').parent().removeClass('col-md-6').addClass('col-md-12');

             var never_table = $('#never').DataTable({
                "scrollY": '556px',
                "info":false,
                "paging": false,
                "lengthChange": false,
                "order": [[ 0, "asc" ]]

           });

           this.never_table = never_table;
          $('#never_filter').parent().prev().remove();
          $('#never_filter').parent().removeClass('col-md-6').addClass('col-md-12');

             var sometimes_table = $('#sometimes').DataTable({
                "scrollY": '556px',
                "info":false,
                "paging": false,
                "lengthChange": false,
                "order": [[ 0, "asc" ]]

           });

           this.sometimes_table = sometimes_table;
          $('#sometimes_filter').parent().prev().remove();
          $('#sometimes_filter').parent().removeClass('col-md-6').addClass('col-md-12');
         }

  });


Show.Concordance = Concordance.Concordance.extend({});

Show.FooterPanel = Views.FooterPanel.extend({
    });
Show.SingleStep = Views.SingleStep.extend({});
Show.OkDialog = Views.OkDialog.extend({});

return Show;

});
