// ================================
// apps/page/show/show_view.js
// ================================

define(["marionette","app","medium","backbone.syphon","common/views","common/util",
        "tpl!apps/projects/page/show/templates/page.tpl",


  ], function(Marionette,App,MediumEditor,BackboneSyphon,Views,Util,pageTpl){


    var Show = {};

  Show.Page = Marionette.View.extend({
  template:pageTpl,
  editor:"",
events:{
      'click .js-stepbackward' : 'backward_clicked',
      'click .js-stepforward' : 'forward_clicked',
      'click .js-firstpage' : 'firstpage_clicked',
      'click .js-lastpage' : 'lastpage_clicked',
      'click .js-correct' : 'correct_clicked',
      'click .line-text' : 'line_clicked',
      'mouseup .line-text' : 'line_selected',

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
       
        var anchor = $(e.currentTarget).attr('anchor');
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
      line_selected:function(e){
        var selection = window.getSelection().toString();
       
        this.trigger("page:line_selected",selection)
      },
     
      onDomRefresh:function(e){


          var ConcordanceButton = MediumEditor.Extension.extend({
              name: 'concordance',

              init: function () {

                this.button = this.document.createElement('button');
                this.button.classList.add('medium-editor-action');
                this.button.innerHTML = 'Show concordance of (n occurrences)';
                this.button.title = 'Show concordance';

                this.on(this.button, 'click', this.handleClick.bind(this));
              },

              getButton: function () {
                return this.button;
              },

              handleClick: function (event) {
              }
            });


            var CorrectionButton = MediumEditor.Extension.extend({
              name: 'corrections',

              init: function () {



                this.button = this.document.createElement('div');
                this.button.classList.add('medium-editor-action');
                this.button.classList.add('dropdown');
                this.button.setAttribute('id','suggestions-menu');

                var dropdown_button = this.document.createElement('button');
                dropdown_button.setAttribute('data-toggle','dropdown');
                dropdown_button.setAttribute('aria-haspopup','true');
                dropdown_button.setAttribute('aria-expanded','false');
                dropdown_button.setAttribute('id','dropdownMenuButton');

                dropdown_button.innerHTML = 'Correction suggestions <i class="fas fa-caret-down">';
                dropdown_button.title = 'Show Correction suggestions';

                this.button.appendChild(dropdown_button);

                 var dropdown_content = document.createElement('div');
                 dropdown_content.classList.add('dropdown-menu');
                 dropdown_content.setAttribute('id','dropdown-content');
                 dropdown_content.setAttribute('aria-labelledby','dropdownMenuButton');
                 this.button.appendChild(dropdown_content);



                this.on(this.button, 'click', this.handleClick.bind(this));
              },

              getButton: function () {
                return this.button;
              },

              handleClick: function (event) {
              }
            });

        this.editor = new MediumEditor('.line-text', {
            disableReturn: true,
            disableDoubleReturn: true,
            toolbar: {
              buttons: ['concordance','corrections']
            },
            buttonLabels: 'fontawesome', // use font-awesome icons for other buttons
            extensions: {
              'concordance': new ConcordanceButton(),
              'corrections': new CorrectionButton()
            },
               handleClick: function (event) {
                this.classApplier.toggleSelection();
                console.log("ACLSLKD")

                // Ensure the editor knows about an html change so watchers are notified
                // ie: <textarea> elements depend on the editableInput event to stay synchronized
                this.base.checkContentChanged();
              }
        });

      }


})



return Show;

});

