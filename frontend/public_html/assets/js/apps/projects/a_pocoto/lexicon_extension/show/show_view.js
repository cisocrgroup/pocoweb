// ================================
// apps/projects/a_pocoto/lexicon_extension/show/show_view.js
// ================================

define(["marionette","app","jquery-ui","backbone.syphon","common/views","apps/projects/page/show/show_view","apps/projects/concordance/show/show_view",
        "tpl!apps/projects/a_pocoto/lexicon_extension/show/templates/info.tpl",


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
      onDestroy : function(){
         clearInterval(this.interval);
      }
  });


  Show.Header = Views.Header.extend({
    initialize: function(){
      }
  });



  Show.LexiconExtension = Marionette.View.extend({
      template: infoTpl,
      events:{
      'click tr' : 'row_clicked',
      'click .js-le-profile' : 'start_le_profile_clicked',
      'click .js-le-redo' : 'le_redo_clicked',

      }, 
    //   serializeData: function(){

    //      var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
    //      console.log(data)
    //     return data;
    
    // },
    serializeData: function(){
      return {
        le: Marionette.getOption(this,"le")
      }
    },
     

      row_clicked:function(e){
        e.preventDefault();
        var word = $($(e.currentTarget).find("td")[0]).text();

        this.trigger("show:word_clicked",word);
      },

         start_le_profile_clicked:function(e){
        e.preventDefault();
        var extensions = [];
        $('#extensions tbody tr').each(function(index) {
          var word = $($(this).find('td')[0])
          extensions.push(word.text());
        });

        this.trigger("show:start_le_profile_clicked",extensions);
      },
       le_redo_clicked:function(e){
        e.preventDefault();
   
        this.trigger("show:le_redo_clicked");
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

Show.SingleStep = Views.SingleStep.extend({});

return Show;

});

