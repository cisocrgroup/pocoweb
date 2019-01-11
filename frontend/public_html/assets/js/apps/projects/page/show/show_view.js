// ================================
// apps/page/show/show_view.js
// ================================

define(["marionette","app","medium","backbone.syphon","common/views","common/util",
        "tpl!apps/projects/page/show/templates/page.tpl",


  ], function(Marionette,App,MediumEditor,BackboneSyphon,Views,Util,pageTpl){


    var Show = {};

  Show.Page = Marionette.View.extend({
  template:pageTpl,
events:{
      'click .js-stepbackward' : 'backward_clicked',
      'click .js-stepforward' : 'forward_clicked',
      'click .js-firstpage' : 'firstpage_clicked',
      'click .js-lastpage' : 'lastpage_clicked',
      'click .js-correct' : 'correct_clicked',
      'click .line-text' : 'line_clicked' 
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
        // var id = $(e.currentTarget).attr('id');
        // console.log(id)
        var anchor = $(e.currentTarget).attr('anchor');

          // Util.toggleFromInputToText(anchor);

          var ids = Util.getIds(anchor);
          var text = $('#line-text-'+anchor).text();
          this.trigger("page:correct_line",{pid:ids[0],page_id:ids[1],line_id:ids[2],text:text},anchor)

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
      onDomRefresh:function(e){

        var ConcordanceButton = MediumEditor.Extension.extend({
              name: 'concordance',

              init: function () {

                this.button = this.document.createElement('button');
                this.button.classList.add('medium-editor-action');
                this.button.innerHTML = 'Show concordance of n occurrences</i>';
                this.button.title = 'Show concordance';

                this.on(this.button, 'click', this.handleClick.bind(this));
              },

              getButton: function () {
                return this.button;
              },

              handleClick: function (event) {
                this.classApplier.toggleSelection();

                // Ensure the editor knows about an html change so watchers are notified
                // ie: <textarea> elements depend on the editableInput event to stay synchronized
                this.base.checkContentChanged();
              }
            });


            var CorrectionButton = MediumEditor.Extension.extend({
              name: 'corrections',

              init: function () {

  
                this.button = this.document.createElement('button');
                this.button.classList.add('medium-editor-action');
                this.button.classList.add('dropdown');
                this.button.classList.add('nav-item');




                this.button.innerHTML = 'Correction suggestions <i class="fas fa-caret-down"></i>';
                this.button.title = 'Show Correction suggestions';

                this.on(this.button, 'click', this.handleClick.bind(this));
              },

              getButton: function () {
                return this.button;
              },

              handleClick: function (event) {
                this.classApplier.toggleSelection();

                // Ensure the editor knows about an html change so watchers are notified
                // ie: <textarea> elements depend on the editableInput event to stay synchronized
                this.base.checkContentChanged();
              }
            });

        var editor = new MediumEditor('.line-text', {
            disableReturn: true,
            disableDoubleReturn: true,
            toolbar: {
              buttons: ['concordance','corrections']
            },
            buttonLabels: 'fontawesome', // use font-awesome icons for other buttons
            extensions: {
              'concordance': new ConcordanceButton(),
              'corrections': new CorrectionButton()
            }
        });

      }


})



return Show;

});

