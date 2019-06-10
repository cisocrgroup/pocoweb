// ================================
// apps/projects/a_pocoto/protocol/show/show_view.js
// ================================

define(["marionette","app","jquery-ui","backbone.syphon","common/views","apps/projects/page/show/show_view","apps/projects/concordance/show/show_view",
        "tpl!apps/projects/a_pocoto/protocol/show/templates/info.tpl",


  ], function(Marionette,App,jquery_ui,BackboneSyphon,Views,Page,Concordance,infoTpl){


    var Show = {};

  Show.Layout = Views.Layout.extend({
  });


  Show.Header = Views.Header.extend({
    initialize: function(){
      }
  });



  Show.Info = Marionette.View.extend({
      template: infoTpl,
      events:{
      'click tr' : 'row_clicked',

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
     

      row_clicked:function(e){
        e.preventDefault();
        var word = $($(e.currentTarget).find("td")[0]).text();

        this.trigger("show:word_clicked",word);
      },

   
     onAttach: function(){
       // var table = $('#book_table').DataTable();
        $(".sortable").sortable({
            items: 'tbody > tr',
            connectWith: ".sortable",

            placeholder: "placeholder",
            delay: 150
          })
          .disableSelection()
          // .dblclick( function(e){
          //   var item = e.target;
          //   if (e.currentTarget.id === 'extensions') {
          //     //move from all to user
          //     $(item).fadeOut('fast', function() {
          //       $(item).appendTo($('#unknown')).fadeIn('slow');
          //     });
          //   } else {
          //     //move from user to all
          //     $(item).fadeOut('fast', function() {
          //       $(item).appendTo($('#extensions')).fadeIn('slow');
          //     });
          //   }
          // });
         }
    
  });


Show.Concordance = Concordance.Concordance.extend({});

Show.FooterPanel = Views.FooterPanel.extend({
    });

return Show;

});

