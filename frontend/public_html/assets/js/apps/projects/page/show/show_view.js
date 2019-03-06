// ================================
// apps/page/show/show_view.js
// ================================

define(["marionette","app","medium","backbone.syphon","common/views","common/util",
        "tpl!apps/projects/page/show/templates/page.tpl",


  ], function(Marionette,App,MediumEditor,BackboneSyphon,Views,Util,pageTpl){


    var Show = {};

  Show.Page = Marionette.View.extend({
  template:pageTpl,
  saved_selection:"",
  editor:"",
events:{
      'click .js-stepbackward' : 'backward_clicked',
      'click .js-stepforward' : 'forward_clicked',
      'click .js-firstpage' : 'firstpage_clicked',
      'click .js-lastpage' : 'lastpage_clicked',
      'click .js-correct' : 'correct_clicked',
      'click .line-text' : 'line_clicked',
      'mouseup .line-text' : 'line_selected',
      'mouseover .line-tokens' : 'tokens_hovered',
      'mouseleave .line-text-parent' : 'line_left',

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
        var text = $('#line-'+anchor).find('.line').text().trim();
        this.trigger("page:correct_line",{pid:ids[0],page_id:ids[1],line_id:ids[2],text:text},anchor)
      },
      tokens_hovered:function(e){
         $('.line-tokens').show();
         $('.line').hide();
         $(e.currentTarget).hide();
         $(e.currentTarget).prev().show();
      },
      line_left:function(e){
        // console.log("mouseleave")
        
        
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
      

        //  $(e.currentTarget).find('.line').focusout(function() {

        // $(e.currentTarget).find('.line-tokens').show();
        // $(e.currentTarget).find('.line').hide();

        // // $(e.currentTarget).next().find('.correct-btn').hide();
        // $(e.currentTarget).css('border-bottom','1px solid transparent');
        // $(e.currentTarget).css('border-left','1px solid transparent');
        // $(e.currentTarget).css('border-top','1px solid transparent');
        // $(e.currentTarget).css('border-top-left-radius','0rem');
        // $(e.currentTarget).css('border-bottom-left-radius','0rem');
        // $(e.currentTarget).find('.line-tokens').css('display','flex');


        //  });


        
      },
      line_selected:function(e){
        console.log(window.getSelection());
       var selection = window.getSelection().toString();
        if(selection==""||selection==" "){
          return;
        }

        $('#current_selection').removeAttr('id');

        var element = document.createElement("span");
        window.getSelection().getRangeAt(0).surroundContents(element)
        element.id="current_selection";
        
        this.saved_selection = selection;
        $('#selected_token').removeAttr("id");
          // Util.replaceSelectedText(selection);
          console.log(selection);
         this.trigger("page:line_selected",selection,window.getSelection().baseNode)
      },

      onAttach:function(e){

        var that = this;

   

      },
     
      onDomRefresh:function(e){

        var that = this;
     $('.line-img').each(function(index){
       $(this).imagesLoaded( function() {
          

            var line = that.model.get('lines')[index];
            Util.addAlignedLine(line);


        });

     });

        if(this.editor!=""){
          this.editor.destroy();
        }



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
                that.trigger("page:concordance_clicked")
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
                dropdown_button.setAttribute('data-flip','false');

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

        // this.editor = new MediumEditor('.line-text', {
        //     disableReturn: true,
        //     disableDoubleReturn: true,
        //     toolbar: {
        //       buttons: ['concordance','corrections']
        //     },
        //     buttonLabels: 'fontawesome', // use font-awesome icons for other buttons
        //     extensions: {
        //       'concordance': new ConcordanceButton(),
        //       'corrections': new CorrectionButton()
        //     },
        //        handleClick: function (event) {
        //         this.classApplier.toggleSelection();

        //         // Ensure the editor knows about an html change so watchers are notified
        //         // ie: <textarea> elements depend on the editableInput event to stay synchronized
        //         this.base.checkContentChanged();
        //       }
        // });

      },

  setErrorDropdowns: function(res,pid) {
  var ddep = document.getElementById("pcw-error-patterns-dropdown");
  var ddet = document.getElementById("pcw-error-tokens-dropdown");
  if (ddep === null || ddet === null) {
    return;
  }
    if (ddep !== null) {
      this.setErrorPatternsDropdown(pid, ddep, res);
    }
    if (ddet !== null) {
      this.setErrorTokensDropdown(pid, ddet, res);
    }
  
},
setErrorPatternsDropdown : function(pid, dropdown, res) {
  var patterns = {};
  var suggs = res.suggestions || [];
  for (var i = 0; i < suggs.length; i++) {
    var id = suggs[i].pageId + '-' + suggs[i].lineId + '-' +
        suggs[i].tokenId;
    for (var j = 0; j < suggs[i].ocrPatterns.length; j++) {
      var pat = suggs[i].ocrPatterns[j].toLowerCase();
      var set = patterns[pat] || {};
      set[i] = true;
      patterns[pat] = set;
    }
  }
  var counts = [];
  for (var p in patterns) {
    counts.push(
        {pattern: p, count: Object.keys(patterns[p]).length});
  }
  counts.sort(function(a, b) { return b.count - a.count; });
  var onclick = function(pid, c) {
    return function() {
    //  pcw.log(c.pattern + ": " + c.count);
      var pat = encodeURI(c.pattern);
      var href = "concordance.php?pid=" + pid + "&q=" + pat +
          "&error-pattern";
      window.location.href = href;
    };
  };
  for (var ii = 0; ii < counts.length; ii++) {
    var c = counts[ii];
    var a = this.appendErrorCountItem(dropdown, c.pattern, c.count);
    a.onclick = onclick(pid, c);
  }
},

setErrorTokensDropdown : function(pid, dropdown, res) {
  // pcw.log("setErrorsDropdown");
  var tokens = {};
  var suggs = res.suggestions || [];
  for (var i = 0; i < suggs.length; i++) {
    var id = suggs[i].pageId + '-' + suggs[i].lineId + '-' +
        suggs[i].tokenId;
    var tok = suggs[i].token.toLowerCase();
    var set = tokens[tok] || {};
    set[id] = true;
    tokens[tok] = set;
  }
  var counts = [];
  for (var p in tokens) {
    counts.push({token: p, count: Object.keys(tokens[p]).length});
  }
  counts.sort(function(a, b) { return b.count - a.count; });
  var onclick = function(pid, c) {
    return function() {
      // pcw.log(c.token + ": " + c.count);
      var pat = encodeURI(c.token);
      var href = "concordance.php?pid=" + pid + "&q=" + pat;
      window.location.href = href;
    };
  };
  for (var ii = 0; ii < counts.length; ii++) {
    c = counts[ii];
    var a = this.appendErrorCountItem(dropdown, c.token, c.count);
    a.onclick = onclick(pid, c);
  }
},
appendErrorCountItem : function(dropdown, item, count) {
  var li = document.createElement("li");
  var a = document.createElement("a");
  var t = document.createTextNode(item + ": " + count);
  a.appendChild(t);
  li.appendChild(a);
  dropdown.appendChild(li);
  return a;
}




})



return Show;

});

