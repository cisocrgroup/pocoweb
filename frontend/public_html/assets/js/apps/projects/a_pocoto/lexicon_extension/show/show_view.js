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
      'click tbody tr' : 'row_clicked',
      'click .js-le-profile' : 'start_le_profile_clicked',
      'click .js-le-redo' : 'le_redo_clicked',
      'click .page_jump' : 'move_to_table_clicked'
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

        this.trigger("show:word_clicked",word.trim());
      },
      move_to_table_clicked:function(e){
        e.preventDefault();
        e.stopPropagation();
        var word = $(e.currentTarget).parent().text().trim();
        this.trigger("show:move_token",word);

      },

         start_le_profile_clicked:function(e){
        e.preventDefault();
        var data = {yes: {}, no: {}};
        $('#extensions tbody tr').each(function(index) {
          var word= $($(this).find('td')[0])
          data.yes[word.text()] = parseInt($($(this).find('td')[1]).text());
          // extensions.push(word.text());
        });
        $('#unknown tbody tr').each(function(index) {
          var word = $($(this).find('td')[0])
          data.no[word.text()] = parseInt($($(this).find('td')[1]).text());
        });

        this.trigger("show:start_le_profile_clicked",data);
      },
       le_redo_clicked:function(e){
        e.preventDefault();

        this.trigger("show:le_redo_clicked");
      },

     onAttach: function(){
       // var table = $('#book_table').DataTable();

          var that = this;

          var extensions_table = $('#extensions').DataTable({
                "scrollY": '556px',
                "info":false,
                "paging": false,
                "lengthChange": false,
                "order": [[ 0, "asc" ]]

           });

            $('#extensions').addClass('connectUnknown');

        
             var unknown_table = $('#unknown').DataTable({
                "scrollY": '556px',
                "info":false,
                "paging": false,
                "lengthChange": false,
                "order": [[ 0, "asc" ]]

           });

             // $('.dataTables_scrollBody > table').addClass('sortable');


        // $("#unknown").sortable({
        //     items: 'tbody > tr',
        //     connectWith: ".connectUnknown",

        //     placeholder: "placeholder",
        //     delay: 150,
        //      update: function( ) 
        //      {
        //     console.log("do stuff");
        //      // extensions_table.rows().invalidate().draw();
        //      }
        //   })
        //   .disableSelection()

    
  
        
       }

  });


Show.Concordance = Concordance.Concordance.extend({});

Show.FooterPanel = Views.FooterPanel.extend({
    });

Show.SingleStep = Views.SingleStep.extend({});
Show.OkDialog = Views.OkDialog.extend({});

return Show;

});
