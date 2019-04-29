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
      'click .line-tokens' : 'line_tokens_clicked',
      'click .line-text' : 'line_selected',
      'click #pcw-error-tokens-link' : 'error_tokens_clicked',
      'click #pcw-error-patterns-link' : 'error_patterns_clicked',

      'mouseover .line-tokens' : 'tokens_hovered',
      'mouseleave .line-text-parent' : 'line_left',
      'keydown .line' : 'line_edited',

      },

      error_tokens_clicked : function(e){
        e.stopPropagation();
        e.preventDefault();
        $(".custom-popover").remove();
        $(".dropdown-menu").hide();
        $('#pcw-error-tokens-dropdown').toggle();
      },

        error_patterns_clicked : function(e){
        e.stopPropagation();
        e.preventDefault();
        $(".custom-popover").remove();
        $(".dropdown-menu").hide();
        $('#pcw-error-patterns-dropdown').toggle();
      },

      serializeData: function(){
      var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
          data.Util = Util;

        return data;
      },

       backward_clicked:function(e){
        e.stopPropagation();
        e.preventDefault();
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        console.log(data)
        this.trigger("page:new",data.prevPageId);
      },

       forward_clicked:function(e){
        e.stopPropagation();
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        e.preventDefault();
        this.trigger("page:new",data.nextPageId);
      },

       firstpage_clicked:function(e){
         e.stopPropagation();
         var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        e.preventDefault();
        this.trigger("page:new","first");
      },
       lastpage_clicked:function(e){
       e.stopPropagation();
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        e.preventDefault();
        this.trigger("page:new","last");
      },
      correct_clicked:function(e){
      e.stopPropagation();

        var anchor = $(e.currentTarget).attr('anchor');
        var ids = Util.getIds(anchor);
        var text = $('#line-'+anchor).find('.line').text().replace(/\s\s+/g, ' ').trim();

        console.log(text);

        this.trigger("page:correct_line",{pid:ids[0],page_id:ids[1],line_id:ids[2],text:text},anchor)
      },
      tokens_hovered:function(e){
         // $('.line-tokens').show();
         // $('.line').hide();
         // $(e.currentTarget).hide();
         // $(e.currentTarget).prev().show();
      },
      line_left:function(e){
        // console.log("mouseleave")

      },

       line_edited:function(e){
        e.stopPropagation();
        $('.custom-popover').remove();
      },

      line_tokens_clicked:function(e){
        e.preventDefault();
         e.stopPropagation();
       $(".custom-popover").remove();

        $('.line').hide();
        $('.line-tokens').show();
        var line_parent = $(e.currentTarget).parent();

        console.log(line_parent);
        console.log($(e.currentTarget));
        line_parent.find('.line').show();
        $(e.currentTarget).hide();

        $('.correct-btn').hide();
        $('.line-text').css('border-bottom','1px solid transparent');
        $('.line-text').css('border-left','1px solid transparent');
        $('.line-text').css('border-top','1px solid transparent');
        $('.line-text').css('border-top-right-radius','.25rem');
        $('.line-text').css('border-bottom-right-radius','.25rem');


        line_parent.css('border-left','1px solid #ced4da');
        line_parent.css('border-bottom','1px solid #ced4da');
        line_parent.css('border-top','1px solid #ced4da');
        line_parent.css('border-top-right-radius','.0rem');
        line_parent.css('border-bottom-right-radius','.0rem');

        line_parent.next().find('.correct-btn').show();


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
        e.stopPropagation();

        var that = this;

         rangy.init();
         var sel =  rangy.getSelection().toString();
         if(sel==""||sel==" "){
           return;
          }

      console.log(sel);
      if($(e.target).hasClass('line')){
       $(".dropdown-menu").hide();
       $(".custom-popover").remove();

      var btn_group = $('<div class="btn-group"></div>');


      btn_group.append($('<button type="button" id="js-concordance" title="Show concordance" class="btn btn-primary btn-sm"><i class="fas fa-align-justify"></i> Concordance </button>'))
      .append($('<button type="button" title="Show Correction suggestions" id="js-suggestions" class="btn btn-primary btn-sm"> <i class="fas fa-list-ol"></i> Suggestions <i class="fas fa-caret-down"></button>'))

 // btn_group.append($('<button type="button" id="js-concordance" title="Show concordance" class="btn btn-primary">Show concordance of (0 occurrences)</button>'))
 //      .append($('<button type="button" title="Show Correction suggestions" id="js-suggestions" class="btn btn-primary">Correction suggestions <i class="fas fa-caret-down"></button>'))


      var div = $('<div class="custom-popover">')
      .css({
        "left": e.pageX + 'px',
        "top": (e.pageY+35) + 'px'
      })
       .append($('<div><i class="fas fa-caret-up custom-popover-arrow"></i></div>'))
       .append(btn_group)
       .appendTo(document.body);

       $('#js-concordance').on('click',function(){
        that.trigger("page:concordance_clicked",sel,0);
       });

         this.saved_selection = sel;
         // Util.replaceSelectedText(selection);
        //   console.log(selection);
         this.trigger("page:line_selected",sel);
      }
      },

      onAttach:function(e){
        var that = this;

        // remove when clicked somewhere else

          $(document).off().click(function(e)
          {
            e.stopPropagation();

            $(".dropdown-menu").hide();

              var custom_popover = $(".custom-popover");
              if (!custom_popover.is(e.target) && custom_popover.has(e.target).length === 0)
              {
                  custom_popover.remove();
              }
          });




      },
      onDestroy:function(e){
       $(".custom-popover").remove();
      },

      onDomRefresh:function(e){

            var that = this;
         $('.line-img').each(function(index){
		 var img = $(this);
           $(this).imagesLoaded( function() {
                var line = that.model.get('lines')[index];
                if(line!=undefined){
                  Util.addAlignedLine(line);
                }
            });

         });

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
  var that = this;
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
      // var pat = encodeURI(c.pattern);
      var pat = c.pattern;
      // var href = "concordance.php?pid=" + pid + "&q=" + pat +
      //     "&error-pattern";
      // window.location.href = href;
      that.trigger("page:error-patterns-clicked",pid,pat);

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
  var that = this;
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
      // var pat = encodeURI(c.token);
      var pat = c.token;
      // var href = "concordance.php?pid=" + pid + "&q=" + pat;
      // window.location.href = href;
       that.trigger("page:error-tokens-clicked",pid,pat);
    };
  };
  for (var ii = 0; ii < counts.length; ii++) {
    c = counts[ii];
    var a = this.appendErrorCountItem(dropdown, c.token, c.count);
    a.onclick = onclick(pid, c);
  }
},
appendErrorCountItem : function(dropdown, item, count) {
  var a = document.createElement("a");
  var t = document.createTextNode(item + ": " + count);
  a.className = "dropdown-item noselect";
  a.appendChild(t);
  dropdown.appendChild(a);
  return a;
}




})



return Show;

});
