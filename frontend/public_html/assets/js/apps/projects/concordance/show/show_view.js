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
      'click .line-text' : 'line_clicked',
      'mouseup .line-text' : 'line_selected',
      'click .js-select-cor' : 'cor_checked',
      'click .js-suggestions-cor' : 'cor_suggestions',
      'click .js-correct-cor' : 'cor_correct',
      'click .cordiv_container' :'cordiv_clicked',
      'click .js-toggle-selection' :'toggle_selection',
      'click .js-set-correction' :'set_correction',
      'click .js-paginate li' : 'paginate_clicked'
      },


      serializeData: function(){
      var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
      var asModal = Marionette.getOption(this,"asModal");
          data.tokendata =  Marionette.getOption(this,"tokendata");
          data.suggestions =  Marionette.getOption(this,"suggestions");
          data.selection =  Marionette.getOption(this,"selection");
          data.le =  Marionette.getOption(this,"le");

          data.Util = Util;
          data.asModal = asModal;

        return data;
      },



      correct_clicked:function(e){

        var that = this;

          $('.concLine').each(function(){

             var cordiv_left = $(this).find('.cordiv_left');
             if(cordiv_left.find('i').hasClass('fa-check-square')){

              var tokendiv = cordiv_left.parent();

              var pid = tokendiv.attr('projectId');
              var lineid =  tokendiv.attr('lineId');
              var pageid =  tokendiv.attr('pageId');
              var tokenid =  tokendiv.attr('tokenId');

              var token  = tokendiv.text().trim();
              var anchor = $(this).attr('anchor');
              // console.log({pid:pid,page_id:pageid,line_id:lineid,token_id:tokenid,token:token});
              that.trigger("concordance:correct_token",{pid:pid,page_id:pageid,line_id:lineid,token_id:tokenid,token:token},anchor);

            }
          });

      },

      toggle_selection:function(e){

        $('.cordiv_container').each(function(){
          var cordiv_left = $(this).find('.cordiv_left');

        if(cordiv_left.find('i').hasClass('fa-square')){
          cordiv_left.empty();
          cordiv_left.append($('<i class="far fa-check-square"></i>'));
          cordiv_left.parent().css('background-color','#d4edda');
          cordiv_left.parent().css('border-color','#c3e6cb');

         }
        else{
          cordiv_left.empty();
          cordiv_left.append($('<i class="far fa-square"></i>'));
          cordiv_left.parent().css('background-color','#cce5ff');
          cordiv_left.parent().css('border-color','#b8daff');

        }

        })

      },

      set_correction:function(e){

        $('.cordiv_container').each(function(){
          console.log($(this));
          var cordiv_left = $(this).find('.cordiv_left');
          var cordiv = $(this).find('.cordiv');

        if(cordiv_left.find('i').hasClass('fa-check-square')){
          cordiv.text($(".js-global-correction-suggestion").val());
         }


        })


      },

      cor_checked:function(e){
         e.stopPropagation();
        var currentTarget = $(e.currentTarget);

        if(currentTarget.find('i').hasClass('fa-square')){
          currentTarget.empty();
          currentTarget.append($('<i class="far fa-check-square"></i>'));
          currentTarget.parent().css('background-color','#d4edda');
          currentTarget.parent().css('border-color','#c3e6cb');

         }
        else{
          currentTarget.empty();
          currentTarget.append($('<i class="far fa-square"></i>'));
          currentTarget.parent().css('background-color','#cce5ff');
          currentTarget.parent().css('border-color','#b8daff');

        }



      },
      cor_correct:function(e){
        e.stopPropagation();
        console.log($(e.currentTarget));
        var concLine = $(e.currentTarget).parent().parent();
        var tokendiv = $(e.currentTarget).parent();

            var pid = tokendiv.attr('projectId');
            var lineid =  tokendiv.attr('lineId');
            var pageid =  tokendiv.attr('pageId');
            var tokenid =  tokendiv.attr('tokenId');
            var token  = tokendiv.text();
            var anchor = concLine.attr('anchor');
            this.trigger("concordance:correct_token",{pid:pid,page_id:pageid,line_id:lineid,token_id:tokenid,token:token},anchor)
      },
      cor_suggestions:function(e){
         e.stopPropagation();

          if($(e.currentTarget).find('#dropdown-content-conc').length>0){
            $('#dropdown-content-conc').toggle();
          }
          else{

          $('.dropdown').remove();
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

      $(e.currentTarget).find('.cordiv_left').show();
      $(e.currentTarget).find('.cordiv_right').show();


      if($(e.currentTarget).hasClass('cordiv')){
       // $(".custom-popover").remove();

      var checkbox = $('<span class="correction_box"><i class="far fa-square"></i></span>');
    //  $(e.currentTarget).find('span').append(checkbox);

      // btn_group.append($('<div class="input-group-prepend"><div class="input-group-text"><input type="checkbox" id="js-select"></div></div>'))
      // btn_group.append($('<input type="text" class="form-control" id="corinput">'))
      // btn_group.append($('<div class="input-group-append" style="background:white;"><button type="button" class="btn btn-outline-secondary dropdown-toggle"></button><button type="button" class="btn btn-outline-secondary"><i class="far fa-arrow-alt-circle-up"></i></button></div>'))

      // var div = $('<div class="custom-popover">')
      // .css({
      //   "left": e.pageX + 'px',
      //   "top": (e.pageY+35) + 'px'
      // })
      //  .append($('<div><i class="fas fa-caret-up custom-popover-arrow"></i></div>'))
      //  .append(btn_group)
      //  .appendTo(document.body);

      //  $('#js-concordance').on('click',function(){
      //   that.trigger("page:concordance_clicked",sel);
      //  });


      }
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
      paginate_clicked: function(e){
        e.preventDefault();


        var text = $(e.currentTarget).text().trim();
        console.log(text)

        if(text=="Next"){
          var next = $(e.currentTarget).parent().find('.active').next();
          if(next.text().trim()=="Next"){
           return;
           }else {
          $(e.currentTarget).parent().find('.active').removeClass("active");
          next.addClass('active');
          }
        }
        else if(text=="Previous"){
           var prev = $(e.currentTarget).parent().find('.active').prev();
          if(prev.text().trim()=="Previous"){
           return;
           }else {
          $(e.currentTarget).parent().find('.active').removeClass("active");
          prev.addClass('active');
          }
        }
        else{
        $(e.currentTarget).parent().find('.active').removeClass("active");
        $(e.currentTarget).addClass('active');
        }


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

                     $('#dropdown-content-conc .dropdown-item').on('click',function(e){
                      // e.stopPropagation();
                      var split = $(this).text().split(" ");
                      $(this).parent().parent().parent().prev().text(split[0]);
                      $(this).parent().hide();
                     })
     },

    onAttach : function(){

      var that = this;

      var timer = 0;
      var delay = 200;
      var prevent = false;
      var after_db_click = false;




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


       }

$('#conc-modal').imagesLoaded( function() {

  $('#conc-modal').css('opacity','1').show();

          var tokendata =  Marionette.getOption(that,"tokendata");
          var suggestions =  Marionette.getOption(that,"suggestions");
          var selection =  Marionette.getOption(that,"selection");
          var le =  Marionette.getOption(that,"le");


           for (key in tokendata['matches']) {

            for (var i=0;i<tokendata['matches'][key].length;i++){

            var match = tokendata['matches'][key][i];
            var line = match['line'];
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

                var token = linetokens[j];

                var whitespace_width=0;
                if (token.ocr.includes(" ")){
                  whitespace_width = token.box.width;
                }

                var box = token['box'];

                // var line_img = document.getElementById(img_id);
                // var boxwidth = box.width + whitespace_length;
                // console.log(line_img);

                // console.log(token);

                // var c = document.createElement("canvas");
                // c.width=boxwidth;
                // c.height=box.height;

                //   var ctx = c.getContext("2d");
                //   ctx.drawImage(line_img, offset, 0, boxwidth, box.height, 0, 0, boxwidth, box.height);

                //     var img = new Image();
                //     img.src = c.toDataURL();
                //     img.setAttribute('id','splitImg_'+line['pageId']+"_"+line['lineId']+"_"+cnt);
                //     img.height = '25';

                    $('#splitImg_'+line['lineId']+"_"+cnt).css('width','auto');

                    var tokendiv;
                    var cordiv = $("<div>"+token.cor+"</div>");

                    if(querytoken.toLowerCase()==token.cor.toLowerCase()){

                        var contenteditable = 'true';
                        if(le){
                          contenteditable = 'false'
                         }


                       cordiv = $("<div class='cordiv' contenteditable='"+contenteditable+"'>"+token.cor.trim()+"</div>");

                       //var grp = $ ("<div class='input-group-mb-3'></div>");
                       // grp.append($("<span class='concbtn_left'><i class='far fa-square'></i></span>"));
                       // grp.append($("<span class='cortoken' contenteditable='true'>"+token.cor.trim()+"</span>"));
                       // grp.append($("<span class='concbtn_right'><i class='fas fa-caret-down'></i></span>"));

                       // cordiv.find('span').append(grp);
                      //
                   //  cordiv = $("<div style='color:green;'>"+token.cor.trim()+"</div>");

                    tokendiv = $('<div class="tokendiv cordiv_container"></div>')
                    tokendiv.append($("<span class='cordiv_left js-select-cor'><i class='cor_item far fa-square'></i></span>")).append(cordiv);;
                    tokendiv.append($("<span class='cordiv_right js-suggestions-cor'><i class='far fa-caret-square-down cor_item'></i></span><span class='cordiv_right js-correct-cor'><i class='cor_item far fa-arrow-alt-circle-up'></i></span>"));

                    }

                    else{
                      tokendiv = $('<div class="tokendiv"></div>').append(cordiv);
                    }

                        tokendiv.attr('pageId',token['pageId']).attr('lineId',token['lineId']).attr('projectId',token['projectId']).
                       attr('tokenId',token['tokenId']);
                    // var div = $("<div style='display:inline-block;'></div>").append(img).append(cordiv);
                    $('#img_'+line['pageId']+"_"+line['lineId']+"_parent").parent().find('.concLine').append(tokendiv);



                    cnt++;

                       // if (token.ocr.includes(" ")){


                        var whitespace_div_length = token.box.width*scalefactor ;
                         cordiv.css('width',whitespace_div_length);


                        prevdiv = cordiv;
                       //  current_position+=(prev_div_width + whitespace_div_length);
                       // }


               }
           // $("#"+img_id).remove();
      }

     }

          // remove when clicked somewhere else
         $(that.el).click(function(e)
          {
              var container = $(".cordiv");
                if (!container.is(e.target) && container.has(e.target).length === 0)
                {
                    container.parent().find('.cordiv_left').hide();
                    container.parent().find('.cordiv_right').hide();
                }
          });

    that.$el.modal('show');




});







  }

})



return Show;

});
