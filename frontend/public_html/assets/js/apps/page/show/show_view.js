// ================================
// apps/page/show/show_view.js
// ================================

define(["marionette","app","backbone.syphon","common/views","common/util",
        "tpl!apps/page/show/templates/page.tpl",


  ], function(Marionette,App,BackboneSyphon,Views,Util,pageTpl){


    var Show = {};

  Show.Page = Marionette.View.extend({
  template:pageTpl,
events:{
      'click .js-stepbackward' : 'backward_clicked',
      'click .js-stepforward' : 'forward_clicked',
      'click .js-firstpage' : 'firstpage_clicked',
      'click .js-lastpage' : 'lastpage_clicked',
      'click .line-text' : 'line_clicked',
      'click .js-correct' : 'correct_clicked'

      },

      serializeData: function(){
      var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
          data.Util = Util;

        return data;
      },

       backward_clicked:function(e){
        e.preventDefault();
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        console.log(data)
        this.trigger("page:new",data.prevPageId);
      },

       forward_clicked:function(e){
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        e.preventDefault();
        this.trigger("page:new",data.nextPageId);
      },

       firstpage_clicked:function(e){
         var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        e.preventDefault();
        this.trigger("page:new","first");
      },
       lastpage_clicked:function(e){
             var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        e.preventDefault();
        this.trigger("page:new","last");
      },
      correct_clicked:function(e){
       
       console.log($(e.currentTarget))
        var id = $(e.currentTarget).attr('id');
        console.log(id)
        var split = id.split("-btn");
        var anchor = split[0];
          Util.toggleFromInputToText(anchor);

          var ids = Util.getIds(anchor);
          this.trigger("page:correct_line",{pid:ids[0],page_id:ids[1],line_id:ids[2],text:$('#'+anchor).val()},anchor)

      },
      line_clicked:function(e){
        e.preventDefault();
        var anchor = $(e.currentTarget).attr('anchor');
        console.log(anchor) 
        Util.toggleFromTextToInput(anchor)


        
      }

//href="page.php?u=none&p=first&pid=', $pid,'"

})



return Show;

});

