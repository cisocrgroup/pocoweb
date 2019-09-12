// ================================
// apps/page/show/show_view.js
// ================================

define(["marionette","app","backbone.syphon","common/views","common/util","apps/projects/concordance/show/show_view",
        "tpl!apps/projects/page/show/templates/page.tpl",
        "tpl!apps/projects/page/show/templates/sidebar.tpl",
        "tpl!apps/projects/page/show/templates/layout.tpl",


  ], function(Marionette,App,BackboneSyphon,Views,Util,Concordance,pageTpl,sidebarTpl,layoutTpl){


    var Show = {};

  Show.Layout = Marionette.View.extend({
    template:layoutTpl,
    regions:{
       headerRegion: "#header-region"
      ,sidebarRegion: "#sidebar-region"
      ,pageRegion: "#page-region"
      ,footerRegion: "#footer-region"
    }

  });


  Show.Header = Views.Header.extend({
    initialize: function(){
        this.title = Marionette.getOption(this,"title"),
        this.icon ="fas fa-book-open"
        this.color ="green"
      }
  });


  Show.Sidebar = Marionette.View.extend({
      template:sidebarTpl,
      events:{
      'click .js-stepbackward' : 'backward_clicked',
      'click .js-stepforward' : 'forward_clicked',
      'click .js-firstpage' : 'firstpage_clicked',
      'click .js-lastpage' : 'lastpage_clicked',
      'click .js-page-link' : 'page_link_clicked',
      'click .suspicious-words tr' : 'error_tokens_clicked',
      'click .error-patterns tr' : 'error_patterns_clicked',
	    'click .special-characters tr' : 'special_characters_clicked',
      'mouseover .special-characters tr' : 'special_characters_hover'

      },
         serializeData: function(){
          var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
          data.lineheight = Marionette.getOption(this,'lineheight');
          data.pagehits = Marionette.getOption(this, 'pagehits');
          data.project = Marionette.getOption(this,"project");

        return data;
      },

      backward_clicked:function(e){
        e.stopPropagation();
        e.preventDefault();
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);

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
       page_link_clicked:function(e){
       e.stopPropagation();
       e.preventDefault();
       console.log();
       this.trigger("page:new",$(e.currentTarget).attr('pageid'));
       $('#pageId').text("Page "+$(e.currentTarget).attr('pageid'));

       },
        error_tokens_clicked : function(e){
        e.stopPropagation();
        e.preventDefault();
        $(".custom-popover").remove();
        var tr = $(e.currentTarget);
        var td = tr.find('td');
        var pat = $(td[0]).html();
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        if(pat!=undefined){
            this.trigger("sidebar:error-tokens-clicked",data.projectId,pat);
        }
      },

        error_patterns_clicked : function(e){
        e.stopPropagation();
        e.preventDefault();
        $(".custom-popover").remove();
        var tr = $(e.currentTarget);
        var td = tr.find('td');
        var pat = $(td[0]).html();
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        if(pat!=undefined){
            this.trigger("sidebar:error-patterns-clicked",data.projectId,pat);
        }
      },
      special_characters_hover : function(e){
		  var f = function(str) {
			  if (!str || str === "") {
				  return "";
			  }
			  return str.split('').map(function(t) {
				  return '\\u' + ('000' + t.charCodeAt(0).toString(16)).substr(-4)
			  }).join('');
		  };
          e.stopPropagation();
          e.preventDefault();
		  var tr = $(e.currentTarget);
		  var c = $(tr.find('td')[0]).html();
		  if (!c || c === "") {
			  return;
		  }
		  $(tr).attr("title",
					 "click to add '" + c + "' to the clipboard or type: '" + f(c) + "'");

	  },
       special_characters_clicked : function(e){
        e.stopPropagation();
        e.preventDefault();
        $(".custom-popover").remove();
        var tr = $(e.currentTarget);
        var td = tr.find('td');
        var pat = $(td[0]).html();
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        if(pat!=undefined){
             tr.fadeOut(200,function(){
                      tr.fadeIn(200,function(){
                      });
                    });
            this.trigger("sidebar:special-characters-clicked",data.projectId,pat);
        }
      },
      onAttach:function(){
        var that = this;

          $('#pageDP').dropdown();

        // $('#pageId').click(function(){
        // });

        $('#sidebar_tabs a').on('click', function (e) {
          e.preventDefault()
            $(this).tab('show',function(){
            });

            $(this).on('shown.bs.tab', function (e) {
                 that.ep_table.draw();
                 that.sp_table.draw();
                 that.char_table.draw();

            })

          })

        var pagehits = Backbone.Marionette.getOption(this, 'pagehits');
        var phslider = document.getElementById("page_hits_slider");

        $('#page_hits_slider').val(pagehits);

        phslider.oninput = function() {
          that.trigger("sidebar:update_page_hits",this.value);
          $('#page_hits_value').text(this.value);
        };

        var lineheight = Backbone.Marionette.getOption(this,'lineheight');
         var slider = document.getElementById("line_size_slider");

         $('#line_size_slider').val(lineheight);

            slider.oninput = function() {
              $('.line-img > img').height(this.value);
               $('.line-tokens').empty();

                $('.line-img').each(function(index){
                  var img = $(this);

                      var line = that.model.get('lines')[index];
                      if(line!=undefined){
                        Util.addAlignedLine(line);
                      }

               });
                $('#lineheight_value').text(this.value+"px");
                that.trigger("sidebar:update_line_height",this.value);

            }
             var linenumbers = Backbone.Marionette.getOption(this,'linenumbers');
              if(linenumbers){
                $('#line_nr_toggle').val(this.checked);
                $('.line-nr').show();
              }
              else{
                $('#line_nr_toggle').prop( "checked", false );
                $('.line-nr').hide();

              }

             $('#line_nr_toggle').change(function() {

                   if(this.checked) {
                    linenumbers=true;
                  }
                  else {
                    linenumbers=false;
                  }
                   $('.line-nr').toggle();

                  that.trigger("sidebar:update_line_numbers",linenumbers);

            });


                 $('#cor_toggle').change(function() {
                   $('.line-text-parent').toggle();

            });



      }

  });

  Show.Page = Marionette.View.extend({
  template:pageTpl,
  saved_selection:"",
  events:{
      'click .js-correct' : 'correct_clicked',
      'click .line-tokens' : 'line_tokens_clicked',
      'click .line-text' : 'line_selected',
      'mouseover .line-tokens' : 'tokens_hovered',
      'mouseleave .line-text-parent' : 'line_left',
      'keydown .line' : 'line_edited',

      },



      serializeData: function(){
      var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
          data.lineheight = Backbone.Marionette.getOption(this,'lineheight');
          data.Util = Util;

        return data;
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
         console.log($(e.target));

         var clicked_token = "";

         if(!$(e.target).hasClass("line-tokens")){
         clicked_token = $(e.target).text();
         }

        $(".custom-popover").remove();

        $('.line').hide();
        $('.line-tokens').show();
        var line_parent = $(e.currentTarget).parent();


        var displayed_line = line_parent.find('.line')
        displayed_line.show();

        console.log(displayed_line.text());
        console.log(clicked_token);


        var obj = displayed_line.get(0);
        console.log(obj);

        // pre-select token:: to do :fix..

        // if(clicked_token!= " "||clicked_token!=""){
        //     var start = displayed_line.text().trim().indexOf(clicked_token);

        //     if (start != -1){

        //       var end = start + clicked_token.length-1;

        //       console.log("start "+start+" end "+end);
        //         var endNode, startNode = endNode = obj.firstChild

        //       startNode.nodeValue = startNode.nodeValue.trim();

        //       var range = document.createRange();
        //       range.setStart(startNode, start);
        //       range.setEnd(endNode, end + 1);

        //       var sel = window.getSelection();
        //       sel.removeAllRanges();
        //       sel.addRange(range);

        //        this.openCustomPopover(e,clicked_token,line_parent);
        //     }
        // }

        $(e.currentTarget).hide();

        // $('.correct-btn').hide();
        // $('.line-text').css('border-bottom','1px solid transparent');
        // $('.line-text').css('border-left','1px solid transparent');
        // $('.line-text').css('border-top','1px solid transparent');
        // $('.line-text').css('border-top-right-radius','.25rem');
        // $('.line-text').css('border-bottom-right-radius','.25rem');


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

      sel = sel.trim();
      console.log(sel);

      if($(e.target).hasClass('line')){

      that.openCustomPopover(e,sel,$(e.target).parent());

      }
      },

      onAttach:function(e){
        var that = this;

        // remove when clicked somewhere else

          $(document).click(function(e)
          {

              e.stopPropagation();

              var custom_popover = $(".custom-popover");
              if (!custom_popover.is(e.target) && custom_popover.has(e.target).length === 0)
              {
                  custom_popover.remove();
              }

              $('.line').hide();
              $('.line-tokens').show();
              $('.correct-btn').hide();
              $('.line-text').css('border','')

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

          /*  sticky sidebar

          var navbar = $("#sidebar-region")
          var sticky = navbar.offset().top;
          var parent_width = navbar.innerWidth();

        // $(window).on('scroll', function(event) {

        //   // console.log(sticky);

        //    if (window.pageYOffset >= sticky) {
        //       $('#sidebar-region').addClass('sticky').width(parent_width);

        //   } else{
        //       $('#sidebar-region').removeClass('sticky');
        //   }
        // });

        */



      },
      openCustomPopover:function(e,sel,parent_div){
        console.log(e)
        var that = this;
         $(".custom-popover").remove();

      var btn_group = $('<div class="btn-group"></div>');


      var suggestions_btn  = $('<div class="dropdown"><button type="button" title="Show Correction suggestions" id="js-suggestions" class="btn btn-primary btn-sm btn dropdown-toggle noselect" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false"> <i class="fas fa-list-ol"></i> Suggestions <i class="fas fa-caret-down"></button></div>')

      btn_group.append($('<button type="button" id="js-concordance" title="Show concordance" class="btn btn-primary btn-sm noselect"><i class="fas fa-align-justify"></i> Concordance </button>'))
      .append(suggestions_btn);

       var dropdown_content = $('<div></div>');
       dropdown_content.addClass('dropdown-menu');
       dropdown_content.attr('id','suggestionsDropdown');
       dropdown_content.attr('aria-labelledby','js-suggestions');
       suggestions_btn.append(dropdown_content);
 // btn_group.append($('<button type="button" id="js-concordance" title="Show concordance" class="btn btn-primary">Show concordance of (0 occurrences)</button>'))
 //      .append($('<button type="button" title="Show Correction suggestions" id="js-suggestions" class="btn btn-primary">Correction suggestions <i class="fas fa-caret-down"></button>'))

      var div = $('<div class="custom-popover">')
      // .css({
      //   "left": e.pageX + 'px',
      //   // "top": (e.pageY+35) + 'px'
      // })
       .append($('<div><i class="fas fa-caret-up custom-popover-arrow"></i></div>'))
       .append(btn_group)
       .appendTo(parent_div); // append to lineparent

      var offset = $('.custom-popover').offset().left;
      var left = e.pageX -offset - ($('.custom-popover').width()/2);
      if(left < 0) left = 0;

      $('.custom-popover').css('left',left+"px");

      var offset_arrow = $('.custom-popover-arrow').offset().left;
      var left_arrow = ($('.custom-popover').width()/2) - (offset_arrow -e.pageX);
      if(left_arrow!=0){
        $('.custom-popover-arrow').css('left',left_arrow+"px");
      }

       $('#js-concordance').on('click',function(){
        that.trigger("page:concordance_clicked",sel,0);
       });

         this.saved_selection = sel;
         // Util.replaceSelectedText(selection);
        //   console.log(selection);
         this.trigger("page:line_selected",sel);
      }




})

Show.Concordance = Concordance.Concordance.extend({});

Show.FooterPanel = Views.FooterPanel.extend({
  manual:true
});

return Show;

});
