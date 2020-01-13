// ================================
// apps/concordance/show/show_view.js
// ================================

define(["marionette","app","imagesLoaded","backbone.syphon","common/views","common/util",
        "tpl!apps/projects/concordance/show/templates/concordance.tpl",


  ], function(Marionette,App,ImagesLoaded,BackboneSyphon,Views,Util,concordanceTpl){


    var Show = {};

  Show.Concordance = Marionette.View.extend({
  template:concordanceTpl,
  le : false,
  events:{
      'click .js-correct-conc' : 'correct_clicked',
      'click .js-select-cor' : 'cor_checked',
      'click .js-suggestions-cor' : 'cor_suggestions',
      'click .js-correct-cor' : 'cor_correct',
      'click .cordiv_container' :'cordiv_clicked',
      'click .js-toggle-selection' :'toggle_selection',
      'click .js-set-correction' :'set_correction',
      'click .js-paginate li' : 'paginate_clicked',
      'keyup .js-global-correction-suggestion' : 'cor_input',
      'click .page_jump' : 'jump_to_page'
      },


      serializeData: function(){
      var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
      var asModal = Marionette.getOption(this,"asModal");
          data.tokendata =  Marionette.getOption(this,"tokendata");
          data.suggestions =  Marionette.getOption(this,"suggestions");
          data.selection =  Marionette.getOption(this,"selection");
          data.le =  Marionette.getOption(this,"le");
          data.lineheight =  Marionette.getOption(this,"lineheight");
          data.linenumbers =  Marionette.getOption(this,"linenumbers");

          data.Util = Util;
          data.asModal = asModal;

        return data;
      },

      jump_to_page: function(e){
        e.stopPropagation();

        var pageId = $(e.currentTarget).attr('pageid');
        var lineId = $(e.currentTarget).attr('lineid');
        var pid = $(e.currentTarget).attr('pid');

         this.trigger("concordance:jump_to_page",{
                  pid:pid,
                  pageId:pageId,
                  lineId:lineId,});

      },

      cor_input : function(e){
        var new_cor = $(".js-global-correction-suggestion").val();
        if(new_cor=="") new_cor = Marionette.getOption(this,"selection");
         $('.cordiv_container').each(function(){
          if($(this).hasClass('cor_selected')){
            $(this).find('.cordiv').text(new_cor);
          }
        });
      },

      correct_clicked:function(e){

        var that = this;
        var checked_length  =   $('.concLine').find('.fa-check-square').length;
        var current_input = $(".js-global-correction-suggestion").val();

          $('.concLine').each(function(){
             var cordiv_left = $(this).find('.cordiv_left');
            if(cordiv_left.find('i').hasClass('fa-check-square')){

              var anchor = $(this).attr('anchor');
              cordiv_left.parent().each(function(i, tokendiv) {
                var pid = tokendiv.attributes.projectid.value;
                var pageid = tokendiv.attributes.pageid.value;
                var lineid = tokendiv.attributes.lineid.value;
                var tokenid = tokendiv.attributes.tokenid.value;
                var token = $(tokendiv).find('.cordiv').text().trim();
                that.trigger("concordance:correct_token",{
                  pid:pid,
                  page_id:pageid,
                  line_id:lineid,
                  token_id:tokenid,
                  token:token}, anchor, function(){
                    checked_length--;
                    if (checked_length == 0){
                       that.trigger("concordance:update_after_correction",
                        {pid:pid,query:Marionette.getOption(that,"selection"),current_input:current_input});
                    }
                  });
              });
            }
          });

      },

      toggle_selection:function(e){

        $('.cordiv_container').toggleClass("cor_selected");

        $('.cordiv_container').each(function(){
          var cordiv_left = $(this).find('.cordiv_left');

        if($(this).hasClass('cor_selected')){
          cordiv_left.empty();
          cordiv_left.append($('<i class="far fa-check-square"></i>'));
          cordiv_left.parent().css('background-color','#cce5ff');
          cordiv_left.parent().css('border-color','#b8daff');
          $('.js-toggle-selection').empty().html('<i class="fas fa-toggle-on"></i> Toggle selection');

         }
        else{
          cordiv_left.empty();
          cordiv_left.append($('<i class="far fa-square"></i>'));
          cordiv_left.parent().css('background-color','#dedede');
          cordiv_left.parent().css('border-color','#bec1c3');
          $('.js-toggle-selection').empty().html('<i class="fas fa-toggle-off"></i> Toggle selection');

        }

        })

      },

      set_correction:function(e){
          var that = this;

        $('.cordiv_container').each(function(){
          // console.log($(this));
          var cordiv_left = $(this).find('.cordiv_left');
          var cordiv = $(this).find('.cordiv');
        if(cordiv_left.find('i').hasClass('fa-check-square')){
           var cor = $(".js-global-correction-suggestion").val();
           if (cor == ""){
            cor = Marionette.getOption(that,'selection');
          }
          cordiv.text(cor);
         }


        })


      },

      cor_checked:function(e){
         e.stopPropagation();
         var currentTarget = $(e.currentTarget);
         currentTarget.parent().toggleClass("cor_selected");

        if(currentTarget.find('i').hasClass('fa-square')){
          currentTarget.empty();
          currentTarget.append($('<i class="far fa-check-square"></i>'));
          currentTarget.parent().css('background-color','#cce5ff');
          currentTarget.parent().css('border-color','#b8daff');

         }
        else{
          currentTarget.empty();
          currentTarget.append($('<i class="far fa-square"></i>'));
          currentTarget.parent().css('background-color','#dedede');
          currentTarget.parent().css('border-color','#bec1c3');

        }



      },
      cor_correct:function(e){
        e.stopPropagation(); 

        var current_input = $(".js-global-correction-suggestion").val();
        var concLine = $(e.currentTarget).parent().parent();
        var tokendiv = $(e.currentTarget).parent();
            var that = this;
            var pid = tokendiv.attr('projectId');
            var lineid =  tokendiv.attr('lineId');
            var pageid =  tokendiv.attr('pageId');
            var tokenid =  tokendiv.attr('tokenid');
            var token  = tokendiv.find('.cordiv').text();
            var anchor = concLine.attr('anchor');

            this.trigger("concordance:correct_token",{pid:pid,page_id:pageid,line_id:lineid,token_id:tokenid,token:token}
              ,anchor
              ,function(){
                       that.trigger("concordance:update_after_correction",
                        {pid:pid,query:Marionette.getOption(that,"selection"),current_input:current_input});
               });
      },
      cor_suggestions:function(e){
         e.stopPropagation();

          if($(e.currentTarget).find('#dropdown-content-conc').length>0){
            $('#dropdown-content-conc').toggle();
          }
          else{

          $('.js-suggestions-cor').find('.dropdown').remove();
            var concLine = $(e.currentTarget).parent().parent();
            var tokendiv = $(e.currentTarget).parent();
            var pid = tokendiv.attr('projectId');
            var lineid =  tokendiv.attr('lineId');
            var pageid =  tokendiv.attr('pageId');
            var tokenid =  tokendiv.attr('tokenId');
            var token  = tokendiv.text();
            var anchor = concLine.attr('anchor');

         
            this.trigger("concordance:show_suggestions",{pid:pid,page_id:pageid,line_id:lineid,token_id:tokenid,token:token,dropdowndiv:$(e.currentTarget)});
          }

      },
      cordiv_clicked:function(e){
        var le =  Marionette.getOption(this,"le");

        if(le){
          return;
        }

      // $('#dropdown-content-conc').remove();

      $("#conc-modal").find('.cordiv_left').hide();
      $("#conc-modal").find('.cordiv_right').hide();
      var o_width =  $(e.currentTarget).css('width');

      $(e.currentTarget).find('.cordiv_left').show();
      $(e.currentTarget).find('.cordiv_right').show();
      $(e.currentTarget).attr('original_width',o_width);

      $(e.currentTarget).css('width','auto');
      $(e.currentTarget).addClass('cor_active');


      },


      paginate_clicked: function(e){
        e.preventDefault();

       var tokendata =  Marionette.getOption(this,"tokendata");
        var value = parseInt($(e.currentTarget).attr('value'));

        if(value==-1) { // value == -1 -> '...' button
          return;
        }

        if(value==-3){ // value == -3 -> 'Next' button
          var next = $(e.currentTarget).parent().find('.active').next();
          if(next.text().trim()=="Next"){
            return;
           } else {
             value = parseInt(next.attr('value'));
          }
        }
        if(value==-2){ // value == -2 -> 'Previous' button
          var prev = $(e.currentTarget).parent().find('.active').prev();
          if(prev.text().trim()=="Previous"){
            return;
          }else {
            value = parseInt(prev.attr('value'));
          }
        }

       var max_pages = Math.ceil(tokendata.total / tokendata.max);
       //console.log("max "+max_pages)
       // console.log(value);

       // case at the end
        if(value>5&&(value+3)>=max_pages){

          $('.js-paginate').empty();

          $('.js-paginate').append('<li value="-2" class="page-item"><a class="page-link" href="#">Previous</a></li>');
          $('.js-paginate').append('<li value="-1" class="page-item"><a class="page-link">...</a></li>');
           for(let k=value-3;k<value;k++){
              $('.js-paginate').append('<li value="'+k+'"" class="page-item"><a class="page-link" href="#">'+k+'</a></li>');
           }
          for(let k=value;k<max_pages;k++){
              $('.js-paginate').append('<li value="'+k+'"" class="page-item"><a class="page-link" href="#">'+k+'</a></li>');
           }


          $('.js-paginate').append('<li value="'+(max_pages)+'" class="page-item"><a class="page-link" href="#">'+(max_pages)+'</a></li>');
          $('.js-paginate').append('<li value="-3" class="page-item"><a class="page-link" href="#">Next</a></li>');

        }


        // in the middle
        else if(value>=5&&value<(max_pages-1)){

          $('.js-paginate').empty();

          $('.js-paginate').append('<li value="-2" class="page-item"><a class="page-link" href="#">Previous</a></li>');
          $('.js-paginate').append('<li value="1" class="page-item"><a class="page-link" href="#">1</a></li>');

          $('.js-paginate').append('<li value="-1" class="page-item"><a class="page-link">...</a></li>');
          for(let k=value-2;k<value;k++){
              $('.js-paginate').append('<li value="'+k+'"" class="page-item"><a class="page-link" href="#">'+k+'</a></li>');
           }
           for(let k=value;k<value+2;k++){
              $('.js-paginate').append('<li value="'+k+'"" class="page-item"><a class="page-link" href="#">'+k+'</a></li>');
           }

          $('.js-paginate').append('<li value="-1" class="page-item"><a class="page-link">...</a></li>');
          $('.js-paginate').append('<li value="'+(max_pages)+'" class="page-item"><a class="page-link" href="#">'+(max_pages)+'</a></li>');
          $('.js-paginate').append('<li value="-3" class="page-item"><a class="page-link" href="#">Next</a></li>');

        }

        // at the start
        else if (value<=5&&(max_pages>5)) {
          $('.js-paginate').empty();

          $('.js-paginate').append('<li value="-2" class="page-item"><a class="page-link" href="#">Previous</a></li>');
          $('.js-paginate').append('<li value="1" class="page-item"><a class="page-link" href="#">1</a></li>');

          for(var k=2;k<=5;k++){
              $('.js-paginate').append('<li value="'+k+'"" class="page-item"><a class="page-link" href="#">'+k+'</a></li>');
           }


          $('.js-paginate').append('<li value="-1" class="page-item"><a class="page-link">...</a></li>');
          $('.js-paginate').append('<li value="'+(max_pages)+'" class="page-item"><a class="page-link" href="#">'+(max_pages)+'</a></li>');
          $('.js-paginate').append('<li value="-3" class="page-item"><a class="page-link" href="#">Next</a></li>');
        }

        $(e.currentTarget).parent().find('.active').removeClass("active");
        $('.js-paginate').find('li[value='+value+']').addClass('active');


        this.trigger("concordance:pagination",value);

      },

     setSuggestionsDropdown : function(div,suggestions){

                var dropdown = $('<span class="dropdown"></span>');
                 div.append(dropdown);

                var suggestions_btn = $('<span class="dropdown-toggle" id="dropdownMenuConc" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false"></span>');
                dropdown.append(suggestions_btn);
                suggestions_btn.attr('aria-haspopup','true');
                suggestions_btn.attr('aria-expanded','false');
                suggestions_btn.attr('id','dropdownMenuConc');
                suggestions_btn.attr('data-flip','true');
                suggestions_btn.attr('data-target','dropdown-content-conc');

                 var dropdown_content = $('<div></div>');
                 dropdown_content.addClass('dropdown-menu');
                 dropdown_content.attr('id','dropdown-content-conc');
                 dropdown_content.attr('aria-labelledby','dropdownMenuConc');
                 suggestions_btn.parent().append(dropdown_content);

                    
                      if(_.isEmpty(suggestions)){
                         $('#dropdown-content-conc').append($('<a class="dropdown-item noselect">No suggestions available</a>"'));
                      }

                     for(i=0;i<suggestions.length;i++){
        						 var s = suggestions[i];
        						 var content = Util.formatSuggestion(s);
                     $('#dropdown-content-conc').append($('<a class="dropdown-item noselect">'+content+"</a>"));
                     }


                    for(key in suggestions){
                       for (var i=0;i<suggestions[key].length;i++){
                        // to do: datatable instead ?

                     var s = suggestions[key][i];
						         var content = Util.formatSuggestion(s);
                     $('#dropdown-content-conc').append($('<a class="dropdown-item noselect">'+content+"</a>"));
                     }
                   }

                   dropdown_content.show();

                    if(!_.isEmpty(suggestions)){

                       $('#dropdown-content-conc .dropdown-item').on('click',function(e){
                        // e.stopPropagation();
                        var split = $(this).text().split(" ");
                        $(this).parent().parent().parent().prev().text(split[0]);
                        // $(this).parent().hide();
                       })
                }
     },

  setImages : function(max){
      var that = this;

     $('.all_lines_parent').empty(); // remove alle lines and images
     var tokendata =  Marionette.getOption(that,"tokendata");
     var lineheight =  Marionette.getOption(that,"lineheight");
     var linenumbers =  Marionette.getOption(that,"linenumbers");

     let nmatches=0;

   for (key in tokendata['matches']) {
            for (var i=0;i<tokendata['matches'][key].lines.length;i++){
            var match = tokendata['matches'][key][i];
            var line = tokendata['matches'][key].lines[i];

             nmatches ++;


        var link = "#/projects/"+line['projectId']+"/page/"+line['pageId'];
        var line_container = $('<div class="line-container">');
        var line_nr = $('<div class="line-nr line-nr-conc" title="page '+line.pageId+ ', line ' + line.lineId+'"> <span>'+line.pageId+":"+line['lineId']+'</span> </div>');

        var text_image_line = $('<div class="text-image-line" title="page '+line.pageId+ ', line ' + line.lineId+'">');

        var left_div = $('<div class="left_div div_inline">');
        var invisible_link = $('<div class="invisible=link" href="'+link+'">');

        var line_img = $('<div id ="img_'+line['pageId']+'_'+line['lineId']+'_parent" class="line-img">');
        var img = $('<img src="'+line['imgFile']+'" id="img_'+line['pageId']+'_'+line['lineId']+'" width="auto" height='+lineheight+'/>');

        // console.log($("#img_"+line['pageId']+"_"+line['lineId']).length);

        if ($("#img_"+line['pageId']+"_"+line['lineId']).length!=0) {
          return;
        }

        left_div.append(invisible_link);
        invisible_link.append(line_img);
        invisible_link.append(img);
        text_image_line.append(left_div);
        line_container.append(line_nr);
        line_container.append(text_image_line);

        var line_jump =$('<div class="page_jump" pid="'+line['projectId']+'" pageid="'+line['pageId']+'" lineid="'+line['lineId']+'" title="Jump to page '+line['pageId']+', line '+line['lineId']+'"> <span><i class="fas fa-chevron-right"></i></span></div>');
        line_container.append(line_jump);

        $('.all_lines_parent').append(line_container);


        }
      };

      if(!linenumbers){
        $('.line-nr').hide();
      }

      var paginated_item = $('.js-paginate').find('.active').val();

      var to = tokendata.skip + nmatches;//*paginated_item;
      $('.concordance-number').text("Showing " + (tokendata.skip+1) + " to " + to +" of " + tokendata.total +" search results");


  },

  setContent : function (init){
      var that = this;

    $('#conc-modal').imagesLoaded( function() {

    if(init){
      $('#conc-modal').css('opacity','1').show();
    }
          var tokendata =  Marionette.getOption(that,"tokendata");
          var suggestions =  Marionette.getOption(that,"suggestions");
          var selection =  Marionette.getOption(that,"selection");
          var le =  Marionette.getOption(that,"le");

          console.log(tokendata);

           for (key in tokendata['matches']) {

            for (var i=0;i<tokendata['matches'][key].lines.length;i++){

            var line = tokendata['matches'][key].lines[i];
            var linetokens = line['tokens'];
            var concLine = $('<div class="concLine"></div>')
            var anchor = line['projectId']+"-"+line['pageId']+"-"+line['lineId'];
            concLine.attr('anchor',anchor);

            var querytoken = key;

            $('#img_'+line['pageId']+"_"+line['lineId']+"_parent").parent().append(concLine);

            var cnt = 0;
            var img_id = "img_"+line['pageId']+"_"+line['lineId'];
            var line_img = document.getElementById(img_id);

            var scalefactor = line_img.width / line.box.width;
            var prevdiv;
             // linecor = Util.replace_all(linecor,word['cor'],'<span class="badge badge-pill badge-primary">'+word['cor']+'</span>');


              for(var j=0;j<linetokens.length;j++) {
                let token = linetokens[j];
                if (j > 0) {
                  let width = linetokens[j].box.left - linetokens[j-1].box.right;
                  let div = $('<div></div>');
                  div.addClass('tokendiv');
                  div.css('width', width * scalefactor);
                  $('#img_'+line['pageId']+"_"+line['lineId']+"_parent").parent().find('.concLine').append(div);
                }
                var tokendiv;
                var cordiv = $("<div>"+token.cor.trim()+"</div>");

                 if(token.isAutomaticallyCorrected){
                  cordiv.addClass('automatically_corrected').addClass('token-text');
                 }
                  if(token.isManuallyCorrected){
                  cordiv.addClass('manually_corrected').addClass('token-text');
                 }

                if (token.match) {
                  var contenteditable = 'true';
                  if(le){
                    contenteditable = 'false'
                  }
                    cordiv = $("<div class='cordiv' contenteditable='"+contenteditable+"'>"+token.cor.trim()+"</div>");
                    tokendiv = $('<div class="tokendiv cordiv_container token-text"></div>')
                    tokendiv.append($("<span title='mark token' class='cordiv_left js-select-cor'><i class='cor_item far fa-square'></i></span>")).append(cordiv);;
                    tokendiv.append($("<span title='show suggestions' class='cordiv_right js-suggestions-cor'><i class='far fa-caret-square-down cor_item'></i></span><span title='correct token' class='cordiv_right js-correct-cor'><i class='cor_item far fa-arrow-alt-circle-up'></i></span>"));

                        if(token.isAutomaticallyCorrected){
                          cordiv.addClass('automatically_corrected').css('border','1px solid #fdd380').css('border-radius','.25rem');
                         }

                    }

                    else{
                      tokendiv = $('<div class="tokendiv"></div>').append(cordiv);
                    }

                tokendiv.attr('pageId',token.pageId)
                .attr('lineId',token.lineId)
                .attr('projectId',token.projectId)
                .attr('tokenId',token.tokenId)
                .attr('offset', token.offset);
                 $('#img_'+line['pageId']+"_"+line['lineId']+"_parent").parent().find('.concLine').append(tokendiv);

                 if(!token.match){ // skip absolute width if match
                 tokendiv.css('width',token.box.width * scalefactor);
                 }

               }
      }

     }

     that.toggle_selection();



});


  },

    onAttach : function(){

      var that = this;

      var timer = 0;
      var delay = 200;
      var prevent = false;
      var after_db_click = false;
      var linenumbers = Marionette.getOption(this,"linenumbers");

      if(!linenumbers){
        $('.line-nr').hide();
      }

       if(this.options.asModal){

          this.$el.attr("id","conc-modal");
          this.$el.addClass("modal fade conc-modal");

          $('#conc-modal').on('show.bs.modal', function () {
            })
            $('#conc-modal').on('hidden.bs.modal', function () {
            })

             $('#conc-modal').on('shown.bs.modal', function () {


                   // $(".cordiv")
                   //  .on("click", function() {
                   //    var that = this;

                   //    if(!after_db_click){

                   //    timer = setTimeout(function() {
                   //      if (!prevent) {
                   //         console.log('single click '+prevent);
                   //         $(that).toggleClass('cor_selected');
                   //         console.log($(that));
                   //         $(that).attr('contenteditable','false');

                   //      }
                   //      prevent = false;
                   //    }, delay);

                   //    }
                   //    else {
                   //      after_db_click = false;
                   //    }

                   //  })
                   //  .on("dblclick", function() {
                   //    clearTimeout(timer);
                   //    prevent = true;
                   //    console.log('double click '+prevent);
                   //    $(this).attr('contenteditable','true');
                   //    after_db_click = true;
                   //    $(this).click();
                   //  });


            })

    // remove when clicked somewhere else
         $(that.el).click(function(e)
          {
              var container = $(".cordiv");
                if (!container.is(e.target) && container.has(e.target).length === 0)
                {
                    container.parent().find('.cordiv_left').hide();
                    container.parent().find('.cordiv_right').hide();
                    var o_width = $('.cor_active').attr('original_width');
                    $('.cor_active').css('width',o_width);
                    $('.cor_active').removeClass('cor_active');
                    $('.cor_active').removeAttr('original_width');

                }
          });

    that.$el.modal('show');

         that = this;
           $(this).imagesLoaded( function() {
             that.setContent(true); // insert Concordance Content
           });
       }

  }



})



return Show;

});
