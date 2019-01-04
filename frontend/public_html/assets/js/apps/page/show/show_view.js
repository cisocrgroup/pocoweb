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
      'click .line-text' : 'line_clicked'
      },

      serializeData: function(){
      var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
          data.Util = Util;

        return data;
      },

       backward_clicked:function(e){
        e.preventDefault();
        this.trigger("page:backward");
      },

       forward_clicked:function(e){
        e.preventDefault();
        this.trigger("page:forward");
      },

       firstpage_clicked:function(e){
        e.preventDefault();
        this.trigger("page:firstpage");
      },
       lastpage_clicked:function(e){
        e.preventDefault();
        this.trigger("page:lastpage");
      },
      line_clicked:function(e){
        e.preventDefault();
        console.log(e)
      }

//href="page.php?u=none&p=first&pid=', $pid,'"

})



return Show;

});

