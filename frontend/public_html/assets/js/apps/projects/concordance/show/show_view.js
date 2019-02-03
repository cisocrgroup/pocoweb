// ================================
// apps/concordance/show/show_view.js
// ================================

define(["marionette","app","medium","backbone.syphon","common/views","common/util",
        "tpl!apps/projects/concordance/show/templates/concordance.tpl",


  ], function(Marionette,App,MediumEditor,BackboneSyphon,Views,Util,concordanceTpl){


    var Show = {};

  Show.Concordance = Marionette.View.extend({
  template:concordanceTpl,
  editor:"",
events:{
      'click .js-stepbackward' : 'backward_clicked',
      'click .js-stepforward' : 'forward_clicked',
      'click .js-firstconcordance' : 'firstconcordance_clicked',
      'click .js-lastconcordance' : 'lastconcordance_clicked',
      'click .js-correct' : 'correct_clicked',
      'click .line-text' : 'line_clicked',
      'mouseup .line-text' : 'line_selected',

      },


      serializeData: function(){
      var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
      console.log(data);
      var asModal = Marionette.getOption(this,"asModal");
          data.tokendata =  Marionette.getOption(this,"tokendata");
          data.suggestions =  Marionette.getOption(this,"suggestions");
          data.Util = Util;
          data.asModal = asModal;

        return data;
      },



       backward_clicked:function(e){
        e.preventDefault();
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        console.log(data)
        this.trigger("concordance:new",data.prevPageId);
      },

       forward_clicked:function(e){
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        e.preventDefault();
        this.trigger("concordance:new",data.nextPageId);
      },

       firstconcordance_clicked:function(e){
         var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        e.preventDefault();
        this.trigger("concordance:new","first");
      },
       lastconcordance_clicked:function(e){
             var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        e.preventDefault();
        this.trigger("concordance:new","last");
      },
      correct_clicked:function(e){
       
        var anchor = $(e.currentTarget).attr('anchor');
          var ids = Util.getIds(anchor);
          var text = $('#line-text-'+anchor).text();
          this.trigger("concordance:correct_line",{pid:ids[0],concordance_id:ids[1],line_id:ids[2],text:text},anchor)

      },
      line_clicked:function(e){
        e.preventDefault();
        $('.correct-btn').hide();
        $('.line-text').css('border-bottom','1px solid transparent');
        $('.line-text').css('border-left','1px solid transparent');
        $('.line-text').css('border-top','1px solid transparent');
        $('.line-text').css('border-top-left-radius','0rem');
        $('.line-text').css('border-bottom-left-radius','0rem');

  
        $(e.currentTarget).css('border-left','1px solid #ced4da');
        $(e.currentTarget).css('border-bottom','1px solid #ced4da');
        $(e.currentTarget).css('border-top','1px solid #ced4da');
        $(e.currentTarget).css('border-top-left-radius','.25rem');
        $(e.currentTarget).css('border-bottom-left-radius','.25rem');

        $(e.currentTarget).next().find('.correct-btn').show();
        
      },
      line_selected:function(e){
        var selection = window.getSelection().toString();
        this.trigger("concordance:line_selected",selection)
      },
     

    onAttach : function(){
      var that = this;
       if(this.options.asModal){

          this.$el.attr("id","conc-modal");
          this.$el.addClass("modal fade conc-modal");
        
          $('#conc-modal').on('show.bs.modal', function () {
                that.trigger("conc:destroy:editor")
            })
            $('#conc-modal').on('hidden.bs.modal', function () {
            })
           this.$el.modal('show');
         
   
       }
  }

})



return Show;

});

